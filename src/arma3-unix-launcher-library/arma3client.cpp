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

std::string ARMA3Client::PickModName(const Mod &mod, const std::vector<std::string> &names)
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

#ifndef DOCTEST_CONFIG_DISABLE
//GCOV_EXCL_START
#include "doctest.h"
#include "tests.hpp"

class ARMA3ClientTests
{
    public:
        ARMA3ClientTests()
        {
            std::filesystem::create_directory(work_dir);
        }

        ~ARMA3ClientTests()
        {
            std::filesystem::remove_all(work_dir);
        }

        std::string dir = Tests::Utils::GetWorkDir() + "/test-files";
        std::string work_dir = Tests::Utils::GetWorkDir() + "/arma3-client-tests";
};

TEST_CASE_FIXTURE(ARMA3ClientTests, "Constructor")
{
    ARMA3Client a3c(dir + "/arma3", dir + "/arma3/!workshop", true);
    CHECK_THROWS_AS(ARMA3Client(dir + "/farma3", dir + "/farma3/!workshop", true), FileNotFoundException);
}

TEST_CASE_FIXTURE(ARMA3ClientTests, "RefreshMods")
{
    std::string arma3_dir = "/arma3";
    std::string workshop_dir = "/!workshop";
    std::string mods_dir = dir + arma3_dir + workshop_dir;
    std::string remove_stamina_dir = mods_dir + "/@Remove stamina";
    std::string big_mod_dir = mods_dir + "/@bigmod";

    ARMA3Client a3c(dir + arma3_dir, mods_dir, true);
    a3c.RefreshMods();
    std::vector<Mod> mods{{{remove_stamina_dir, Tests::Utils::remove_stamina_map}, {big_mod_dir, Tests::Utils::big_mod_map}}};
    CHECK_EQ(mods, a3c.mods_workshop_);
}

TEST_CASE_FIXTURE(ARMA3ClientTests, "CreateWorkshopSymlink")
{
    std::string arma3_dir = work_dir + "/arma3";
    std::string workshop_dir = arma3_dir + "/!workshop";
    std::string steam_workshop_dir = dir + "/steam/steamapps/workshop/content/107410";
    std::string executable_name = EXECUTABLE_NAME;
    std::vector<std::string> ls_result_expected{"@Remove stamina", "@bigmod"};

    CHECK(std::filesystem::create_directory(arma3_dir));
    CHECK(StdUtils::CreateFile(std::filesystem::path(arma3_dir) / executable_name));

    ARMA3Client a3c(arma3_dir, steam_workshop_dir, true);
    CHECK(a3c.CreateSymlinkToWorkshop());
    CHECK(std::filesystem::exists(workshop_dir));
    std::vector<std::string> ls_result_actual = StdUtils::Ls(workshop_dir);
    std::sort(ls_result_actual.begin(), ls_result_actual.end());
    CHECK_EQ(ls_result_expected, ls_result_actual);
}

/*bool CreateArmaCfg(const std::vector<Mod> &mod_list);
bool Start(const std::string &arguments);*/
TEST_CASE_FIXTURE(ARMA3ClientTests, "CreateArmaCfg")
{

}

//GCOV_EXCL_STOP
#endif
