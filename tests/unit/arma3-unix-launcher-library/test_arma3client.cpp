#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include <trompeloeil.hpp>
#include "default_test_reporter.hpp"

#include "arma3client.hpp"
#include "exceptions/file_not_found.hpp"
#include "static_todo.hpp"

#include "cppfilter.hpp"
#include "filesystem_utils.hpp"
#include "mod.hpp"
#include "steam_utils.hpp"
#include "std_utils.hpp"
#include "vdf.hpp"
#include "mock/cppfilter.hpp"
#include "mock/filesystem_utils.hpp"
#include "mock/mod.hpp"
#include "mock/steam_utils.hpp"
#include "mock/std_utils.hpp"
#include "mock/vdf.hpp"

using trompeloeil::_;

class ARMA3ClientTests
{
    public:
        CppFilterMock cppFilterMock;
        FilesystemUtilsMock filesystemUtilsMock;
        ModMock modMock;
        SteamUtilsMock steamUtilsMock;
        StdUtilsMock stdUtilsMock;
        VdfMock vdfMock;

        std::filesystem::path const arma_path = "/arma_path";
        std::filesystem::path const workshop_path = "/workshop_path";
        std::string const mac_executable_name = "ArmA3.app";
        std::string const linux_executable_name = "arma3.x86_64";
        std::string const proton_executable_name = "arma3_x64.exe";

        std::filesystem::path get_executable_path()
        {
            #ifdef __linux__
            return arma_path / linux_executable_name;
            #else
            return arma_path / mac_executable_name;
            #endif
        }
};

#ifdef __linux__
TEST_CASE_FIXTURE(ARMA3ClientTests, "Constructor_Linux")
{
    GIVEN("Arma and workshop path")
    {
        WHEN("Arma path contains ARMA 3 Linux executable")
        {
            REQUIRE_CALL(filesystemUtilsMock, Exists(arma_path / linux_executable_name)).RETURN(true);
            THEN("ARMA3::Client is created")
            {
                ARMA3::Client a3c(arma_path, workshop_path);
            }
        }
        WHEN("Arma path contains ARMA 3 Proton executable")
        {
            REQUIRE_CALL(filesystemUtilsMock, Exists(arma_path / linux_executable_name)).RETURN(false);
            REQUIRE_CALL(filesystemUtilsMock, Exists(arma_path / proton_executable_name)).RETURN(true);
            THEN("ARMA3::Client is created")
            {
                ARMA3::Client a3c(arma_path, workshop_path);
            }
        }
        WHEN("Arma path does not contain ARMA 3 executables")
        {
            REQUIRE_CALL(filesystemUtilsMock, Exists(arma_path / linux_executable_name)).RETURN(false);
            REQUIRE_CALL(filesystemUtilsMock, Exists(arma_path / proton_executable_name)).RETURN(false);
            THEN("Exception is thrown")
            {
                CHECK_THROWS_AS(ARMA3::Client(arma_path, workshop_path), FileNotFoundException);
            }
        }
    }
}
#endif

#ifdef __APPLE__
TEST_CASE_FIXTURE(ARMA3ClientTests, "Constructor_Mac_OS_X")
{
    GIVEN("Arma and workshop path")
    {
        WHEN("Arma path contains ARMA 3 Mac OS X executable")
        {
            REQUIRE_CALL(filesystemUtilsMock, Exists(arma_path / mac_executable_name)).RETURN(true);
            THEN("ARMA3::Client is created")
            {
                ARMA3::Client a3c(arma_path, workshop_path);
            }
        }
        WHEN("Arma path does not contain ARMA 3 executable")
        {
            REQUIRE_CALL(filesystemUtilsMock, Exists(arma_path / mac_executable_name)).RETURN(false);
            THEN("Exception is thrown")
            {
                CHECK_THROWS_AS(ARMA3::Client(arma_path, workshop_path), FileNotFoundException);
            }
        }
    }
}
#endif

#ifdef __linux__
TEST_CASE_FIXTURE(ARMA3ClientTests, "IsProton_Linux")
{
    GIVEN("Arma 3 Client")
    {
        WHEN("Arma path contains ARMA 3 Linux executable")
        {
            REQUIRE_CALL(filesystemUtilsMock, Exists(arma_path / linux_executable_name)).RETURN(true);
            ARMA3::Client a3c(arma_path, workshop_path);
            THEN("isProton() is false")
            {
                CHECK_FALSE(a3c.IsProton());
            }
        }
        WHEN("Arma path contains ARMA 3 Proton executable")
        {
            REQUIRE_CALL(filesystemUtilsMock, Exists(arma_path / linux_executable_name)).RETURN(false);
            REQUIRE_CALL(filesystemUtilsMock, Exists(arma_path / proton_executable_name)).RETURN(true);
            ARMA3::Client a3c(arma_path, workshop_path);
            THEN("isProton() is true")
            {
                CHECK(a3c.IsProton());
            }
        }
    }
}
#endif

#ifdef __APPLE__
TEST_CASE_FIXTURE(ARMA3ClientTests, "IsProton_Mac_OS_X")
{
    GIVEN("Arma 3 Client")
    {
        REQUIRE_CALL(filesystemUtilsMock, Exists(arma_path / mac_executable_name)).RETURN(true);
        ARMA3::Client a3c(arma_path, workshop_path);
        WHEN("Arma path contains ARMA 3 Mac OS X executable")
        {
            THEN("isProton() is false")
            {
                CHECK_FALSE(a3c.IsProton());
            }
        }
    }
}
#endif

TEST_CASE_FIXTURE(ARMA3ClientTests, "GetHomeMods")
{
    GIVEN("Arma's home directory path and ARMA3::Client")
    {
        REQUIRE_CALL(filesystemUtilsMock, Exists(get_executable_path())).RETURN(true);
        ARMA3::Client a3c(arma_path, workshop_path);
        WHEN("Arma's home directory does not contain mods")
        {
            REQUIRE_CALL(filesystemUtilsMock, Ls(arma_path, _)).RETURN(std::vector<std::string> {});
            THEN("Returned vector is empty")
            {
                CHECK(a3c.GetHomeMods().empty());
            }
        }
        WHEN("Arma's home directory contains only excluded directories")
        {
            REQUIRE_CALL(filesystemUtilsMock, Ls(arma_path, _)).RETURN(std::vector<std::string> {"Addons", "Expansion"});
            THEN("Returned vector is empty")
            {
                CHECK(a3c.GetHomeMods().empty());
            }
        }
        WHEN("Arma's home directory contains only some excluded directories and two valid mods")
        {
            std::array<std::string, 2> const mod_names{"@othermod", "@Remove_stamina"};
            std::array<std::filesystem::path, 2> const mod_paths{arma_path / mod_names[0], arma_path / mod_names[1]};

            REQUIRE_CALL(filesystemUtilsMock, Ls(arma_path, _)).RETURN(std::vector<std::string> {"Addons", "Expansion", mod_names[0], mod_names[1]});
            REQUIRE_CALL(filesystemUtilsMock, IsDirectory(mod_paths[0])).RETURN(true);
            REQUIRE_CALL(filesystemUtilsMock, Ls(mod_paths[0], _)).RETURN(std::vector<std::string> {"addons"});
            REQUIRE_CALL(modMock, Constructor(mod_paths[0], _));
            REQUIRE_CALL(filesystemUtilsMock, IsDirectory(mod_paths[1])).RETURN(true);
            REQUIRE_CALL(filesystemUtilsMock, Ls(mod_paths[1], _)).RETURN(std::vector<std::string> {"addons"});
            REQUIRE_CALL(modMock, Constructor(mod_paths[1], _));
            THEN("Returned vector contains two valid mods")
            {
                auto mods = a3c.GetHomeMods();
                CHECK_EQ(mod_paths[0], mods[0].path_);
                CHECK_EQ(mod_paths[1], mods[1].path_);
            }
        }
    }
}

TEST_CASE_FIXTURE(ARMA3ClientTests, "GetWorkshopMods")
{
    GIVEN("Arma's workshop directory path and ARMA3::Client")
    {
        REQUIRE_CALL(filesystemUtilsMock, Exists(get_executable_path())).RETURN(true);
        ARMA3::Client a3c(arma_path, workshop_path);
        WHEN("Workshop directory does not contain mods")
        {
            REQUIRE_CALL(filesystemUtilsMock, Ls(workshop_path, _)).RETURN(std::vector<std::string> {});
            THEN("Returned vector is empty")
            {
                CHECK(a3c.GetWorkshopMods().empty());
            }
        }
        WHEN("Workshop directory contains only two directories with useless files inside")
        {
            std::array<std::string, 2> const mod_names{"123", "456"};
            std::array<std::filesystem::path, 2> const mod_paths{workshop_path / mod_names[0], workshop_path / mod_names[1]};
            REQUIRE_CALL(filesystemUtilsMock, Ls(workshop_path, _)).RETURN(std::vector<std::string> {mod_paths[0], mod_paths[1]});
            REQUIRE_CALL(filesystemUtilsMock, IsDirectory(mod_paths[0])).RETURN(true);
            REQUIRE_CALL(filesystemUtilsMock, Ls(mod_paths[0], _)).RETURN(std::vector<std::string> {"useless.bin"});
            REQUIRE_CALL(filesystemUtilsMock, IsDirectory(mod_paths[1])).RETURN(true);
            REQUIRE_CALL(filesystemUtilsMock, Ls(mod_paths[1], _)).RETURN(std::vector<std::string> {"useless.bin"});
            THEN("Returned vector is empty")
            {
                CHECK(a3c.GetWorkshopMods().empty());
            }
        }
        WHEN("Workshop directory contains two directories with workshop mods inside")
        {
            std::array<std::string, 2> const mod_names{"123", "456"};
            std::array<std::filesystem::path, 2> const mod_paths{workshop_path / mod_names[0], workshop_path / mod_names[1]};
            REQUIRE_CALL(filesystemUtilsMock, Ls(workshop_path, _)).RETURN(std::vector<std::string> {mod_paths[0], mod_paths[1]});
            REQUIRE_CALL(filesystemUtilsMock, IsDirectory(mod_paths[0])).RETURN(true);
            REQUIRE_CALL(filesystemUtilsMock, Ls(mod_paths[0], _)).RETURN(std::vector<std::string> {"addons"});
            REQUIRE_CALL(modMock, Constructor(mod_paths[0], _));
            REQUIRE_CALL(filesystemUtilsMock, IsDirectory(mod_paths[1])).RETURN(true);
            REQUIRE_CALL(filesystemUtilsMock, Ls(mod_paths[1], _)).RETURN(std::vector<std::string> {"addons"});
            REQUIRE_CALL(modMock, Constructor(mod_paths[1], _));
            THEN("Returned vector is empty")
            {
                auto mods = a3c.GetWorkshopMods();
                CHECK_EQ(mod_paths[0], mods[0].path_);
                CHECK_EQ(mod_paths[1], mods[1].path_);
            }
        }
    }
}

TEST_CASE_FIXTURE(ARMA3ClientTests, "CreateArmaCfg")
{
    GIVEN("ARMA3::Client with valid home directory structure and valid mod list")
    {
        std::string const config_file_mod_part = R"cpp(class ModLauncherList
{
    class Mod1
    {
        dir="123";
        name="Remove Stamina";
        origin="GAME DIR";
        fullPath="C:\workshop_path\123";
    };
    class Mod2
    {
        dir="456";
        name="Big Mod";
        origin="GAME DIR";
        fullPath="C:\workshop_path\456";
    };
};
)cpp";
        std::filesystem::path const random_directory = "/random";
        std::filesystem::path const random_config_path = "/random/config.cfg";

        REQUIRE_CALL(filesystemUtilsMock, Exists(get_executable_path())).RETURN(true);
        ARMA3::Client a3c(arma_path, workshop_path);

        std::filesystem::path remove_stamina_path = workshop_path / "123";
        REQUIRE_CALL(modMock, Constructor(remove_stamina_path, _)).SIDE_EFFECT(_2.KeyValue["name"] = "Remove Stamina");

        std::filesystem::path big_mod_path = workshop_path / "456";
        REQUIRE_CALL(modMock, Constructor(big_mod_path, _)).SIDE_EFFECT(_2.KeyValue["name"] = "Big Mod");

        WHEN("Arma config file does not exist but config file's parent directory exists")
        {
            REQUIRE_CALL(filesystemUtilsMock, Exists(random_config_path)).RETURN(false);
            REQUIRE_CALL(filesystemUtilsMock, Exists(random_directory)).RETURN(true);
            REQUIRE_CALL(stdUtilsMock, CreateFile(random_config_path)).RETURN(true);
            REQUIRE_CALL(stdUtilsMock, FileReadAllText(random_config_path)).RETURN("");

            THEN("Config file is created, containing only mods")
            {
                REQUIRE_CALL(cppFilterMock, RemoveClass("class ModLauncherList", _)).RETURN("");
                REQUIRE_CALL(stdUtilsMock, FileWriteAllText(random_config_path, config_file_mod_part));
                a3c.CreateArmaCfg({"/workshop_path/123", "/workshop_path/456"}, random_config_path);
            }
        }

        WHEN("Arma config file does not exist and config file's parent directory does not exist")
        {
            REQUIRE_CALL(filesystemUtilsMock, Exists(random_config_path)).RETURN(false);
            REQUIRE_CALL(filesystemUtilsMock, Exists(random_directory)).RETURN(false);
            REQUIRE_CALL(filesystemUtilsMock, CreateDirectories(random_directory)).RETURN(true);
            REQUIRE_CALL(stdUtilsMock, CreateFile(random_config_path)).RETURN(true);
            REQUIRE_CALL(stdUtilsMock, FileReadAllText(random_config_path)).RETURN("");

            THEN("Config directory is created and config file is created, containing only mods")
            {
                REQUIRE_CALL(cppFilterMock, RemoveClass("class ModLauncherList", _)).RETURN("");
                REQUIRE_CALL(stdUtilsMock, FileWriteAllText(random_config_path, config_file_mod_part));
                a3c.CreateArmaCfg({"/workshop_path/123", "/workshop_path/456"}, random_config_path);
            }
        }

        WHEN("Arma config file exists and contains some entries")
        {
            std::string const entries = R"cpp(setting="one";
someInt=5;
)cpp";
            std::string const expected_config_file = entries + config_file_mod_part;

            REQUIRE_CALL(filesystemUtilsMock, Exists(random_config_path)).RETURN(true);
            REQUIRE_CALL(stdUtilsMock, FileReadAllText(random_config_path)).RETURN(entries);

            THEN("Config directory is created and config file is created, containing mods and previous entries")
            {
                REQUIRE_CALL(cppFilterMock, RemoveClass("class ModLauncherList", _)).RETURN(entries);
                REQUIRE_CALL(stdUtilsMock, FileWriteAllText(random_config_path, expected_config_file));
                a3c.CreateArmaCfg({"/workshop_path/123", "/workshop_path/456"}, random_config_path);
            }
        }
    }
}

#ifdef __linux__
TEST_CASE_FIXTURE(ARMA3ClientTests, "Start_Linux_DirectLaunch")
{
    GIVEN("Arma 3 Client")
    {
        std::string const arguments = "some random arguments";
        std::string_view const working_directory(arma_path.c_str());

        WHEN("Arma path contains ARMA 3 Linux executable")
        {
            std::string const executable_path = arma_path / linux_executable_name;

            REQUIRE_CALL(filesystemUtilsMock, Exists(executable_path)).RETURN(true);
            ARMA3::Client a3c(arma_path, workshop_path);

            WHEN("User environment variables are not specified")
            {
                std::string const launch_command = fmt::format("env  \"{}\" {}", executable_path, arguments);
                REQUIRE_CALL(stdUtilsMock, StartBackgroundProcess(launch_command, working_directory));

                WHEN("esync is disabled")
                {
                    THEN("Arma is started with passed arguments")
                    {
                        a3c.Start(arguments, "", true, false);
                    }
                }
                WHEN("esync is enabled")
                {
                    THEN("Arma is started with passed arguments, disable_esync parameter is discarded, since not using Proton")
                    {
                        a3c.Start(arguments, "", true, true);
                    }
                }
            }

            WHEN("User environment variables are specified")
            {
                std::string const user_environment_variables = "SOME_NICE_ENV_VAR=1";
                std::string const launch_command = fmt::format("env {} \"{}\" {}", user_environment_variables, executable_path, arguments);
                REQUIRE_CALL(stdUtilsMock, StartBackgroundProcess(launch_command, working_directory));

                THEN("Arma is started with passed arguments")
                {
                    a3c.Start(arguments, user_environment_variables, true, false);
                }
            }
        }

        WHEN("Arma path contains ARMA 3 Proton executable")
        {
            std::uint64_t const arma_id = 107410;
            std::string const executable_path = arma_path / proton_executable_name;
            std::filesystem::path const steam_path = "/steam_path";
            std::filesystem::path const proton_path = "/proton_dir/proton";
            std::string const proton_args = "run";

            REQUIRE_CALL(filesystemUtilsMock, Exists(arma_path / linux_executable_name)).RETURN(false);
            REQUIRE_CALL(filesystemUtilsMock, Exists(executable_path)).RETURN(true);
            ARMA3::Client a3c(arma_path, workshop_path);

            REQUIRE_CALL(steamUtilsMock, Constructor(_, _));
            REQUIRE_CALL(steamUtilsMock, GetCompatibilityToolForAppId(arma_id, _)).RETURN(std::pair<std::filesystem::path, std::string>(proton_path , proton_args));
            REQUIRE_CALL(steamUtilsMock, GetSteamPath(_)).RETURN(steam_path);
            REQUIRE_CALL(steamUtilsMock, GetInstallPathFromGamePath(arma_path, _)).RETURN(steam_path);

            REQUIRE_CALL(stdUtilsMock, IsLibraryAvailable(_)).RETURN(true);

            WHEN("esync is disabled")
            {
                auto const ld_preload = getenv("LD_PRELOAD");
                std::string const old_ld_preload = ld_preload ? ld_preload : "";

                WHEN("LD_PRELOAD is not set")
                {
                    unsetenv("LD_PRELOAD");
                    THEN("Arma is started with passed arguments")
                    {
                        constexpr char const* launch_command = R"command(env  SteamGameId=107410 LD_PRELOAD=/steam_path/ubuntu12_64/gameoverlayrenderer.so STEAM_COMPAT_DATA_PATH="/steam_path/steamapps/compatdata/107410"   "/proton_dir/proton" run "/arma_path/arma3_x64.exe" some random arguments)command";

                        REQUIRE_CALL(stdUtilsMock, StartBackgroundProcess(launch_command, working_directory));
                        a3c.Start(arguments, "", true, false);
                    }
                }

                WHEN("LD_PRELOAD is set")
                {
                    setenv("LD_PRELOAD", "somelib.so", true);
                    THEN("Arma is started with passed arguments and LD_PRELOAD value is preserved, along with gameoverlayrenderer.so")
                    {
                        constexpr char const* launch_command = R"command(env  SteamGameId=107410 LD_PRELOAD=/steam_path/ubuntu12_64/gameoverlayrenderer.so:somelib.so STEAM_COMPAT_DATA_PATH="/steam_path/steamapps/compatdata/107410"   "/proton_dir/proton" run "/arma_path/arma3_x64.exe" some random arguments)command";

                        REQUIRE_CALL(stdUtilsMock, StartBackgroundProcess(launch_command, working_directory));
                        a3c.Start(arguments, "", true, false);
                    }
                }

                if (old_ld_preload.empty())
                    unsetenv("LD_PRELOAD");
                else
                    setenv("LD_PRELOAD", old_ld_preload.c_str(), true);
            }

            WHEN("esync is enabled")
            {
                // LD_PRELOAD preservation applies here, but no need to repeat the code
                THEN("Arma is started with passed arguments and PROTON_NO_ESYNC environment variable set to 1")
                {
                    constexpr char const* launch_command = R"command(env PROTON_NO_ESYNC=1 SteamGameId=107410 LD_PRELOAD=/steam_path/ubuntu12_64/gameoverlayrenderer.so STEAM_COMPAT_DATA_PATH="/steam_path/steamapps/compatdata/107410"   "/proton_dir/proton" run "/arma_path/arma3_x64.exe" some random arguments)command";

                    REQUIRE_CALL(stdUtilsMock, StartBackgroundProcess(launch_command, working_directory));
                    a3c.Start(arguments, "", true, true);
                }
            }
        }
    }
}

TEST_CASE_FIXTURE(ARMA3ClientTests, "Start_Linux_IndirectLaunchThroughSteam")
{
    GIVEN("Arma 3 Client")
    {
        std::string const arguments = "some random arguments";
        std::string const steam_command = "steam -applaunch 107410";

        REQUIRE_CALL(steamUtilsMock, Constructor(_, _)).THROW(std::runtime_error(""));

        WHEN("Arma path contains ARMA 3 Linux executable")
        {
            REQUIRE_CALL(filesystemUtilsMock, Exists(arma_path / linux_executable_name)).RETURN(true);
            ARMA3::Client a3c(arma_path, workshop_path);

            WHEN("esync is disabled")
            {
                THEN("Arma is started with passed arguments")
                {
                    std::string const launch_command = fmt::format("env  {} {}", steam_command, arguments);

                    REQUIRE_CALL(stdUtilsMock, StartBackgroundProcess(launch_command, ANY(std::string_view)));
                    a3c.Start(arguments, "", false, false);
                }
            }
            WHEN("esync is enabled")
            {
                THEN("Arma is started with passed arguments, disable_esync parameter is discarded, since not using Proton")
                {
                    std::string const launch_command = fmt::format("env  {} {}", steam_command, arguments);

                    REQUIRE_CALL(stdUtilsMock, StartBackgroundProcess(launch_command, ANY(std::string_view)));
                    a3c.Start(arguments, "", false, true);
                }
            }
        }
        WHEN("Arma path contains ARMA 3 Proton executable")
        {
            REQUIRE_CALL(filesystemUtilsMock, Exists(arma_path / linux_executable_name)).RETURN(false);
            REQUIRE_CALL(filesystemUtilsMock, Exists(arma_path / proton_executable_name)).RETURN(true);
            ARMA3::Client a3c(arma_path, workshop_path);
            WHEN("esync is disabled")
            {
                THEN("Arma is started with passed arguments preceded by -nolauncher option")
                {
                    std::string const launch_command = fmt::format("env   {} -nolauncher {}", steam_command, arguments);

                    REQUIRE_CALL(stdUtilsMock, StartBackgroundProcess(launch_command, ANY(std::string_view)));
                    a3c.Start(arguments, "", false, false);
                }
            }
            WHEN("esync is enabled")
            {
                THEN("Arma is started with passed arguments preceded by -nolauncher option and PROTON_NO_ESYNC environment variable set to 1")
                {
                    std::string const launch_command = fmt::format("env PROTON_NO_ESYNC=1  {} -nolauncher {}", steam_command, arguments);

                    REQUIRE_CALL(stdUtilsMock, StartBackgroundProcess(launch_command, ANY(std::string_view)));
                    a3c.Start(arguments, "", false, true);
                }
            }
        }
    }
}
#endif

#ifdef __APPLE__
TEST_CASE_FIXTURE(ARMA3ClientTests, "Start_Mac_OS_X")
{
    GIVEN("Arma 3 Client")
    {
        std::string const arguments = "some random arguments";
        std::string const arguments_whitespaces_replaced = "some%20random%20arguments";
        std::string const steam_command = "open steam://run/107410//";

        REQUIRE_CALL(filesystemUtilsMock, Exists(get_executable_path())).RETURN(true);
        ARMA3::Client a3c(arma_path, workshop_path);
        THEN("Arma is started with passed arguments")
        {
            std::string const launch_command = fmt::format("env  {}{}", steam_command, arguments_whitespaces_replaced);
            REQUIRE_CALL(stdUtilsMock, StartBackgroundProcess(launch_command, ANY(std::string_view)));
            a3c.Start(arguments, "");
        }
    }
}
#endif
