#include "arma3client.hpp"

#include <algorithm>
#include <filesystem>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <static_todo.hpp>

#include "cppfilter.hpp"
#include "std_utils.hpp"
#include "string_utils.hpp"

#include "exceptions/directory_not_found.hpp"
#include "exceptions/file_no_access.hpp"
#include "exceptions/file_not_found.hpp"
#include "exceptions/not_a_symlink.hpp"
#include "exceptions/syntax_error.hpp"

using namespace std;
using namespace std::filesystem;

using namespace StdUtils;
using namespace StringUtils;

/*
 * fmt weirdly handles std::filesystem::path
 * it wraps whole path in " ", also it displays
 * backslash: R"(\)"
 * as two backslashes: R"(\\)"
 */
namespace fmt
{
    template <>
    struct formatter<std::filesystem::path> : formatter<std::string> {};
}

namespace ARMA3
{
    Client::Client(std::filesystem::path const &arma_path, std::filesystem::path const &target_workshop_path,
                   bool skip_initialization)
    {
        path_ = arma_path;
        path_custom_ = path_ / Definitions::symlink_custom_name;
        path_executable_ = path_ / Definitions::executable_name;
        path_workshop_local_ = path_ / Definitions::symlink_workshop_name;
        path_workshop_target_ = target_workshop_path;
        if (!exists(path_executable_))
            throw FileNotFoundException(path_executable_);

        if (skip_initialization)
            return;

        CreateSymlinkToWorkshop();
        RefreshMods();

        create_directory(path_custom_);
    }

    void Client::CreateArmaCfg(std::vector<Mod> const &mod_list, std::filesystem::path cfg_path = "")
    {
        if (cfg_path.empty())
            cfg_path = GetCfgPath();
        std::string existing_config = FileReadAllText(cfg_path);
        if (!exists(cfg_path.parent_path()))
            throw PathNoAccessException(cfg_path);

        CppFilter cpp_filter{existing_config};
        auto stripped_config = cpp_filter.RemoveClass("class ModLauncherList");
        stripped_config +=
            "class ModLauncherList" "\n"
            "{" "\n";

        constexpr char const *mod_template =
            "    class Mod{}" "\n"
            "    {{" "\n"
            R"(        dir="{}";)" "\n"
            R"(        name="{}";)" "\n"
            R"(        origin="GAME DIR";)" "\n"
            R"(        fullPath="{}";)" "\n"
            "    }};" "\n";

        int mod_number = 1;
        for (auto &mod : mod_list)
        {
            path mod_path_absolute = Trim<std::string, std::string>(mod.path_, "\"");
            path final_path = StringUtils::ToWindowsPath(mod_path_absolute);
            path dir = Trim<std::string, std::string>(mod_path_absolute.filename(), "\"");
            auto name = mod.GetValueOrReturnDefault(dir, "name", "dir", "tooltip");

            stripped_config += fmt::format(mod_template, mod_number, dir, name, final_path);
            ++mod_number;
        }

        stripped_config += "};" "\n";

        FileWriteAllText(cfg_path, stripped_config);
    }

    void Client::Start(std::string const &arguments)
    {
        system(("steam --applaunch 107410 " + arguments).c_str());
    }

    void Client::AddCustomMod(path const &mod_path)
    {
        std::vector<std::string> entities = StdUtils::Ls(mod_path, true);
        if (!Contains(entities, "addons"))
            throw DirectoryNotFoundException(mod_path / "addons");

        Mod mod{mod_path, {}};
        mod.LoadAllCPP();

        std::string link_name = PickModName(mod, std::vector<std::string> {"dir", "name", "tooltip"});
        if (link_name.empty())
            link_name = mod_path.filename();

        if (link_name[0] != '@')
            link_name.insert(link_name.begin(), '@');

        path source_path = path_custom_ / link_name;
        while (exists(source_path))
        {
            directory_entry entry(source_path);
            if (entry.exists() && entry.is_symlink() && read_symlink(source_path) == mod.path_)
                return;
            source_path += '_';
        }

        std::error_code ec;
        create_directory_symlink(mod.path_, source_path, ec);
    }

    void Client::RemoveCustomMod(path const &mod_path)
    {
        if (!exists(mod_path))
            return;

        path custom_mods_path = path_ / Definitions::symlink_custom_name;

        if (!StartsWith(mod_path, custom_mods_path))
            throw PathNoAccessException(mod_path);

        if (!is_symlink(mod_path) && !is_regular_file(mod_path))
            throw NotASymlinkException(mod_path);

        remove(mod_path);
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

    std::string Client::PickModName(Mod const &mod, std::vector<std::string> const &names)
    {
        TODO_BEFORE(03, 2019, "Pick mod name from workshop");
        for (auto const &name : names)
        {
            if (ContainsKey(mod.KeyValue, name))
                return mod.KeyValue.at(name);
        }
        return filesystem::path(mod.path_).filename();
    }

    bool Client::CreateSymlinkToWorkshop()
    {
        if (path_workshop_target_.empty())
            return false;

        bool status = true;

        create_directory(path_workshop_local_);

        for (auto const &entity : filesystem::directory_iterator(path_workshop_target_))
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

            path source_path = path_workshop_local_ / link_name;
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

    void Client::AddModsFromDirectory(std::filesystem::path const &dir, std::vector<Mod> &target)
    {
        if (!exists(dir))
            return;

        for (auto const &ent : StdUtils::Ls(dir))
        {
            if (StdUtils::Contains(Definitions::exclusions, ent))
                continue;
            std::filesystem::path mod_dir = dir / ent;
            if (!is_directory(mod_dir))
                continue;
            Mod m{mod_dir, {}};
            for (auto const &cppfile : StdUtils::Ls(mod_dir))
                if (StringUtils::EndsWith(cppfile, ".cpp"))
                    m.LoadFromFile(mod_dir / cppfile, true);
            target.emplace_back(std::move(m));
        }
    }

    std::filesystem::path Client::GetCfgPath()
    {
        return std::filesystem::path(Definitions::home_directory)
               / Definitions::local_share_prefix
               / Definitions::bohemia_interactive_prefix
               / Definitions::game_config_path;
    }

    std::filesystem::path const &Client::GetPath()
    {
        return path_;
    }

    std::filesystem::path const &Client::GetPathExecutable()
    {
        return path_executable_;
    }

    std::filesystem::path const &Client::GetPathWorkshop()
    {
        return path_workshop_target_;
    }
}

#ifndef DOCTEST_CONFIG_DISABLE
//GCOV_EXCL_START
#include <iostream>

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

        ARMA3ClientFixture(ARMA3ClientFixture const &) = delete;
        ARMA3ClientFixture(ARMA3ClientFixture &&) = delete;
        ARMA3ClientFixture &operator= (ARMA3ClientFixture const &) = delete;
        ARMA3ClientFixture &operator= (ARMA3ClientFixture &&) = delete;

        std::filesystem::path client_tests_dir = work_dir / "arma3-client-tests";
        std::filesystem::path steam_workshop_dir = "steam/steamapps/workshop/content/107410";

        std::filesystem::path absolute_arma3_path = test_files_path / arma3_dir;
        std::filesystem::path testing_dir = client_tests_dir / arma3_dir;
};


namespace doctest
{
    template<> struct StringMaker<std::vector<std::string>>
    {
        static String convert(std::vector<std::string> const &value)
        {
            std::string text = "\"";
            for (auto const &str : value)
                text += str + ",";
            text += "\"";
            return text.c_str();
        }
    };

    template<> struct StringMaker<std::vector<Mod>>
    {
        static String convert(std::vector<Mod> const &value)
        {
            std::string text = "\"";
            for (auto const &mod : value)
                text += std::string(mod) + ",";
            text += "\"";
            return text.c_str();
        }
    };
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
            CHECK_THROWS_AS(ARMA3::Client(test_files_dir / "farma3", test_files_dir / "farma3" / "!workshop", true),
                            FileNotFoundException);
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
                        {absolute_arma3_path / workshop_dir / big_mod_dir, Tests::Utils::big_mod_map}
                    }};
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

TEST_CASE_FIXTURE(ARMA3ClientFixture, "CreateArmaCfg")
{
    std::string config_file_mod_part = "class ModLauncherList" "\n"
                                       "{{" "\n"
                                       "    class Mod1" "\n"
                                       "    {{" "\n"
                                       R"(        dir="@Remove stamina";)" "\n"
                                       R"(        name="Remove Stamina";)" "\n"
                                       R"(        origin="GAME DIR";)" "\n"
                                       R"(        fullPath="{0}\!workshop\@Remove stamina";)" "\n"
                                       "    }};" "\n"
                                       "    class Mod2" "\n"
                                       "    {{" "\n"
                                       R"(        dir="@bigmod";)" "\n"
                                       R"(        name="Big Mod";)" "\n"
                                       R"(        origin="GAME DIR";)" "\n"
                                       R"(        fullPath="{0}\!workshop\@bigmod";)" "\n"
                                       "    }};" "\n"
                                       "}};" "\n";

    GIVEN("ARMA3::Client with valid home directory structure and valid mod list")
    {
        std::filesystem::path config_path = client_tests_dir / "arma3.cfg";
        ARMA3::Client a3c(absolute_arma3_path, absolute_arma3_path / workshop_dir, true);
        std::vector<Mod> mods_workshop{{{absolute_arma3_path / workshop_dir / remove_stamina_dir, Tests::Utils::remove_stamina_map},
                {absolute_arma3_path / workshop_dir / big_mod_dir, Tests::Utils::big_mod_map}
            }};

        WHEN("Arma config file does not exist")
        {
            THEN("Exception is thrown")
            {
                CHECK_THROWS_AS(a3c.CreateArmaCfg(mods_workshop, config_path), filesystem_error);
            }
        }

        std::string windows_style_arma3_path = StringUtils::ToWindowsPath(absolute_arma3_path);
        std::string mod_part = fmt::format(config_file_mod_part, windows_style_arma3_path);

        std::array<std::string, 5> config_files =
        {
            "",
            Tests::Utils::valid_config_file,
            Tests::Utils::valid_config_file + Tests::Utils::mod_classes,
            Tests::Utils::valid_config_file + Tests::Utils::unrelated_classes,
            Tests::Utils::valid_config_file + Tests::Utils::unrelated_classes + Tests::Utils::mod_classes
        };

        std::array<std::string, 5> out_files =
        {
            mod_part,
            Tests::Utils::valid_config_file + mod_part,
            Tests::Utils::valid_config_file + mod_part,
            Tests::Utils::valid_config_file + Tests::Utils::unrelated_classes + mod_part,
            Tests::Utils::valid_config_file + Tests::Utils::unrelated_classes + mod_part
        };

        for (size_t i = 0; i < config_files.size(); ++i)
        {
            WHEN("Creating Arma Config based on item from config_files array")
            {
                StdUtils::FileWriteAllText(config_path, config_files[i]);
                REQUIRE(StdUtils::CreateFile(client_tests_dir / "arma3.cfg"));

                CHECK_NOTHROW(a3c.CreateArmaCfg(mods_workshop, config_path));

                THEN("Arma Config is created, containing valid config from out_files array")
                {
                    REQUIRE_EQ(out_files[i], StdUtils::FileReadAllText(config_path));
                }
            }
        }
    }
}

TEST_CASE_FIXTURE(ARMA3ClientFixture, "Start")
{
    ARMA3::Client a3c(absolute_arma3_path, test_files_path / steam_workshop_dir, true);
    std::string path_env_var = std::string(test_files_path / "steam") + ":" + getenv("PATH");
    chdir(client_tests_dir.c_str());
    setenv("PATH", path_env_var.c_str(), 1);

    path arguments_passed_path = client_tests_dir / "argumentspassed";

    std::array<std::string, 3> arguments_array
    {
        "",
        "-cpuCount=4",
        "-exThreads=7 -cpuCount=4 -world=empty -skipIntro"
    };

    std::array<std::string, 3> call_arguments_array
    {
        "--applaunch 107410",
        "--applaunch 107410 -cpuCount=4",
        "--applaunch 107410 -exThreads=7 -cpuCount=4 -world=empty -skipIntro"
    };

    for (size_t i = 0; i < arguments_array.size(); ++i)
    {
        a3c.Start(arguments_array[i]);
        CHECK_EQ(call_arguments_array[i], FileReadAllText(arguments_passed_path));
        std::filesystem::remove(arguments_passed_path);
    }
}

TEST_CASE_FIXTURE(ARMA3ClientFixture, "AddCustomMod")
{
    auto check_dir_structure_ok = [&](auto const & ls_result_expected)
    {
        REQUIRE(std::filesystem::exists((testing_dir / custom_dir)));
        std::vector<std::string> ls_result_actual = StdUtils::Ls(testing_dir / custom_dir);
        std::sort(ls_result_actual.begin(), ls_result_actual.end());
        CHECK_EQ(ls_result_expected, ls_result_actual);
    };

    GIVEN("List of mods, ARMA3::Client")
    {
        std::vector<std::string> ls_result_expected{"@Remove stamina", "@bigmod"};

        REQUIRE(std::filesystem::create_directory(testing_dir));
        REQUIRE(StdUtils::CreateFile(std::filesystem::path(testing_dir) / ARMA3::Definitions::executable_name));

        ARMA3::Client a3c(testing_dir, test_files_path / steam_workshop_dir);

        WHEN("Adding two custom valid mods")
        {
            a3c.AddCustomMod(test_files_path / steam_workshop_dir / "1");
            a3c.AddCustomMod(test_files_path / steam_workshop_dir / "3");

            THEN("!custom folder is created, symlinks are created")
            {
                check_dir_structure_ok(ls_result_expected);
            }
        }

        WHEN("Adding invalid mod (without 'addons' directory inside)")
        {
            THEN("Exception is thrown")
            {
                CHECK_THROWS_AS(a3c.AddCustomMod(test_files_path / steam_workshop_dir / "2"), DirectoryNotFoundException);
            }
        }
    }

    GIVEN("List of mods, ARMA3::Client, two valid mods aded")
    {
        std::vector<std::string> ls_result_expected{"@Remove stamina"};

        REQUIRE(std::filesystem::create_directory(testing_dir));
        REQUIRE(StdUtils::CreateFile(std::filesystem::path(testing_dir) / ARMA3::Definitions::executable_name));

        ARMA3::Client a3c(testing_dir, test_files_path / steam_workshop_dir);
        a3c.AddCustomMod(test_files_path / steam_workshop_dir / "1");

        check_dir_structure_ok(ls_result_expected);

        WHEN("Adding valid mod again")
        {
            a3c.AddCustomMod(test_files_path / steam_workshop_dir / "1");
            THEN("Nothing will happen")
            {
                check_dir_structure_ok(ls_result_expected);
            }
        }
    }
}

TEST_CASE_FIXTURE(ARMA3ClientFixture, "RemoveCustomMod")
{
    GIVEN("ARMA3::Client")
    {
        REQUIRE(std::filesystem::create_directory(testing_dir));
        REQUIRE(StdUtils::CreateFile(std::filesystem::path(testing_dir) / ARMA3::Definitions::executable_name));

        ARMA3::Client a3c(testing_dir, test_files_path / steam_workshop_dir);

        path custom_dir = testing_dir / "!custom";
        path custom_mod = custom_dir / "@Remove stamina";

        WHEN("Trying to remove mod inside !custom folder")
        {
            REQUIRE_FALSE(exists(custom_mod));
            a3c.AddCustomMod(test_files_path / steam_workshop_dir / "1");
            REQUIRE(exists(custom_mod));
            CHECK_EQ(test_files_path / steam_workshop_dir / "1", read_symlink(custom_mod));

            CHECK_NOTHROW(a3c.RemoveCustomMod(custom_mod));
            THEN("Symlink gets deleted")
            {
                CHECK_FALSE(exists(custom_mod));
            }
        }

        WHEN("Trying to remove file !custom folder")
        {
            REQUIRE(CreateFile(custom_mod));
            CHECK_NOTHROW(a3c.RemoveCustomMod(custom_mod));
            THEN("File gets deleted")
            {
                CHECK_FALSE(exists(custom_mod));
            }
        }

        WHEN("Trying to remove directory from !custom folder")
        {
            REQUIRE(create_directory(custom_mod));
            THEN("Exception is thrown")
            {
                CHECK_THROWS_AS(a3c.RemoveCustomMod(custom_mod), NotASymlinkException);
            }
        }

        WHEN("Trying to remove not-existing mod")
        {
            path not_existing = custom_dir / "@not-existing-mod";
            CHECK_FALSE(exists(not_existing));
            THEN("Nothing happens")
            {
                CHECK_NOTHROW(a3c.RemoveCustomMod(not_existing));
            }
        }

        WHEN("Trying to remove mod outside !custom folder")
        {
            THEN("Exception is thrown")
            {
                CHECK_THROWS_AS(a3c.RemoveCustomMod(testing_dir / "!workshop/@Remove stamina"), PathNoAccessException);
            }
        }
    }
}

TEST_SUITE_END();

//GCOV_EXCL_STOP
#endif
