#include "arma3client.hpp"

#include <algorithm>
#include <filesystem>

#include <fmt/format.h>

#include "std_utils.hpp"
#include "string_utils.hpp"

#include "exceptions/file_not_found.hpp"

using namespace std;
using namespace std::filesystem;

using namespace StdUtils;
using namespace StringUtils;

namespace ARMA3
{
    Client::Client(std::filesystem::path arma_path, std::filesystem::path target_workshop_path, bool skip_initialization)
    {
        path_ = arma_path;
        path_executable_ = path_ / Definitions::executable_name;
        path_workshop_local_ = path_ / Definitions::symlink_workshop_name;
        path_workshop_target_ = target_workshop_path;
        if (!exists(path_executable_))
            throw FileNotFoundException(path_executable_);

        if (skip_initialization)
            return;

        CreateSymlinkToWorkshop();
        RefreshMods();
    }

    bool Client::CreateArmaCfg(const std::vector<Mod> &mod_list, std::filesystem::path cfg_path = "")
    {
        if (cfg_path.empty())
            cfg_path = GetCfgPath();
        return false;
    }

    bool Client::Start(const std::string &arguments)
    {
        return false;
    }

    bool Client::RefreshMods()
    {
        mods_custom_.clear();
        mods_home_.clear();
        mods_workshop_.clear();

        AddModsFromDirectory(path_ / Definitions::symlink_custom_name, mods_custom_);
        AddModsFromDirectory(path_, mods_home_);
        AddModsFromDirectory(path_workshop_local_, mods_workshop_);

        return true;
    }

    std::string Client::PickModName(const Mod &mod, const std::vector<std::string> &names)
    {
        for (const auto &name : names)
        {
            if (ContainsKey(mod.KeyValue, name))
                return mod.KeyValue.at(name);
        }
        return filesystem::path(mod.path_).filename();
    }

    bool Client::CreateSymlinkToWorkshop()
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

            std::string source_path = path_workshop_local_ / link_name;
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

    void Client::AddModsFromDirectory(const std::filesystem::path &dir, std::vector<Mod> &target)
    {
        if (!exists(dir))
            return;

        for (const auto &ent : StdUtils::Ls(dir))
        {
            if (StdUtils::Contains(Definitions::exclusions, ent))
                continue;
            std::filesystem::path mod_dir = dir / ent;
            if (!is_directory(mod_dir))
                continue;
            Mod m{mod_dir, {}};
            for (const auto &cppfile : StdUtils::Ls(mod_dir))
                if (StringUtils::EndsWith(cppfile, ".cpp"))
                    m.LoadFromFile(mod_dir / cppfile, true);
            target.emplace_back(std::move(m));
        }
    }

    std::filesystem::path Client::GetCfgPath()
    {
        fmt::print("outpath: {}", (std::filesystem::path(Definitions::home_directory)
                   / Definitions::local_share_prefix
                   / Definitions::bohemia_interactive_prefix
                   / Definitions::game_config_path).c_str());
        return std::filesystem::path(Definitions::home_directory)
               / Definitions::local_share_prefix
               / Definitions::bohemia_interactive_prefix
               / Definitions::game_config_path;
    }
}

#ifndef DOCTEST_CONFIG_DISABLE
//GCOV_EXCL_START
#include "doctest.h"
#include "tests.hpp"

class ARMA3ClientFixture : public Tests::Fixture
{
public:
    ARMA3ClientFixture()
    {
        std::filesystem::create_directory(client_tests_dir);
    }

    ~ARMA3ClientFixture()
    {
        std::filesystem::remove_all(client_tests_dir);
    }

    std::filesystem::path client_tests_dir = work_dir / "arma3-client-tests";
    std::filesystem::path steam_workshop_dir = "steam/steamapps/workshop/content/107410";

    std::filesystem::path absolute_arma3_path = test_files_path / arma3_dir;
    std::filesystem::path testing_dir = client_tests_dir / arma3_dir;
};

std::ostream& operator<< (std::ostream& os, const std::vector<Mod> value)
{
    os << "\nMod vector begin:\n";
    for (const auto& mod: value)
        os << mod << "-----\n";
    os << "Mod vector end:\n\n";
    return os;
}

TEST_SUITE_BEGIN("ARMA3::Client");

TEST_CASE_FIXTURE(ARMA3ClientFixture, "Constructor")
{
    WHEN("Given existing path containg ARMA 3 executable")
    {
        THEN("ARMA3::Client is created")
        {
            ARMA3::Client a3c(absolute_arma3_path, absolute_arma3_path / workshop_dir, true);
        }
    }

    WHEN("Given path not containg ARMA 3 executable")
    {
        THEN("ARMA3::Client constructor throws exception")
        {
            CHECK_THROWS_AS(ARMA3::Client(test_files_dir / "farma3", test_files_dir / "farma3" / "!workshop", true), FileNotFoundException);
        }
    }
}

TEST_CASE_FIXTURE(ARMA3ClientFixture, "RefreshMods")
{
    GIVEN("ARMA3::Client with valid home directory structure")
    {
        ARMA3::Client a3c(absolute_arma3_path, test_files_path / steam_workshop_dir, true);
        WHEN("Refreshing mod lists")
        {
            a3c.RefreshMods();
            THEN("Mod vectors are set according to installed mods")
            {
                std::vector<Mod> mods_workshop{{{absolute_arma3_path / workshop_dir / remove_stamina_dir, Tests::Utils::remove_stamina_map},
                        {absolute_arma3_path / workshop_dir / big_mod_dir, Tests::Utils::big_mod_map}}};
                CHECK_EQ(mods_workshop, a3c.mods_workshop_);

                std::vector<Mod> mods_custom{{{absolute_arma3_path / custom_dir / rand_mod_v2_dir, Tests::Utils::random_mod_map}}};
                CHECK_EQ(mods_custom, a3c.mods_custom_);

                std::vector<Mod> mods_home{{{absolute_arma3_path / random_mod_dir, Tests::Utils::random_mod_map}}};
                CHECK_EQ(mods_home, a3c.mods_home_);
            }
        }
    }
}

TEST_CASE_FIXTURE(ARMA3ClientFixture, "CreateWorkshopSymlink")
{
    std::vector<std::string> ls_result_expected{"@Remove stamina", "@bigmod"};

    REQUIRE(std::filesystem::create_directory(testing_dir));
    REQUIRE(StdUtils::CreateFile(std::filesystem::path(testing_dir) / ARMA3::Definitions::executable_name));

    ARMA3::Client a3c(testing_dir, test_files_path / steam_workshop_dir, true);
    CHECK(a3c.CreateSymlinkToWorkshop());
    REQUIRE(std::filesystem::exists((testing_dir / workshop_dir)));
    std::vector<std::string> ls_result_actual = StdUtils::Ls(testing_dir / workshop_dir);
    std::sort(ls_result_actual.begin(), ls_result_actual.end());
    CHECK_EQ(ls_result_expected, ls_result_actual);
}

/*bool CreateArmaCfg(const std::vector<Mod> &mod_list);
bool Start(const std::string &arguments);*/
TEST_CASE_FIXTURE(ARMA3ClientFixture, "CreateArmaCfg")
{
    ARMA3::Client a3c(absolute_arma3_path, absolute_arma3_path / workshop_dir, true);

    //a3c.CreateArmaCfg({});
}

TEST_SUITE_END();

//GCOV_EXCL_STOP
#endif
