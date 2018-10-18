#include "arma3client.hpp"

#include <algorithm>
#include <filesystem>

#include "std_utils.hpp"
#include "string_utils.hpp"

#include "exceptions/file_not_found.hpp"

using namespace std;
using namespace std::filesystem;

using namespace StdUtils;
using namespace StringUtils;

ARMA3Client::ARMA3Client(std::string arma_path, std::string target_workshop_path, bool skip_initialization)
{
    path_ = arma_path;
    path_executable_ = path_ + "/" + executable_name_;
    path_workshop_local_ = path_ + "/" + symlink_workshop_name_;
    path_workshop_target_ = target_workshop_path;
    if (!exists(path_executable_))
        throw FileNotFoundException(path_executable_);

    if (skip_initialization)
        return;

    CreateSymlinkToWorkshop();
    RefreshMods();
}

bool ARMA3Client::CreateArmaCfg(const std::vector<Mod> &mod_list)
{
    return false;
}

bool ARMA3Client::Start(const std::string &arguments)
{
    return false;
}

bool ARMA3Client::RefreshMods()
{
    mods_custom_.clear();
    mods_home_.clear();
    mods_workshop_.clear();

    AddModsFromDirectory(path_workshop_local_, mods_workshop_);

    return false;
}

std::string PickModName(const Mod &mod, const std::vector<std::string> &names)
{
    for (const auto &name : names)
    {
        if (ContainsKey(mod.KeyValue, name))
            return mod.KeyValue.at(name);
    }
    return filesystem::path(mod.path_).filename();
}

bool ARMA3Client::CreateSymlinkToWorkshop()
{
    bool status = true;

    create_directory(path_workshop_local_);

    for (const auto &entity : filesystem::directory_iterator(path_workshop_target_))
    {
        if (!entity.is_directory())
            continue;

        path dir = entity.path().filename();
        path mod_dir = path_workshop_target_ / dir;

        std::vector<std::string> entities = StdUtils::Ls(mod_dir, true);
        if (!Contains(entities, "addons"))
            continue;
        Mod m{mod_dir, {}};
        m.LoadAllCPP();

        std::string link_name = PickModName(m, std::vector<std::string> {"dir", "name", "tooltip"});
        if (link_name.empty())
            link_name = dir;

        if (link_name[0] != '@')
            link_name.insert(link_name.begin(), '@');

        std::string source_path = path_workshop_local_ + "/" + link_name;
        directory_entry entry(source_path);
        if (entry.exists() && entry.is_symlink() && read_symlink(source_path) == mod_dir)
            continue;

        remove(source_path);
        std::error_code ec;
        create_directory_symlink(mod_dir, source_path, ec);

        if (ec.value() != 0)
            status = false;
    }

    return status;
}

void ARMA3Client::AddModsFromDirectory(std::string dir, std::vector<Mod> &target)
{
    if (!exists(dir))
        return;

    for (const auto &ent : StdUtils::Ls(dir))
    {
        std::string mod_dir = dir + "/" + ent;
        Mod m{mod_dir, {}};
        for (const auto &cppfile : StdUtils::Ls(mod_dir))
            if (StringUtils::EndsWith(cppfile, ".cpp"))
                m.LoadFromFile(mod_dir + "/" + cppfile, true);
        target.emplace_back(std::move(m));
    }
}
