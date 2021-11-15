#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include <trompeloeil.hpp>
#include "default_test_reporter.hpp"

#include <filesystem>

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "steam_utils.hpp"

#include "filesystem_utils.hpp"
#include "std_utils.hpp"
#include "vdf.hpp"
#include "mock/filesystem_utils.hpp"
#include "mock/std_utils.hpp"
#include "mock/vdf.hpp"

#include "exceptions/directory_not_found.hpp"
#include "exceptions/steam_install_not_found.hpp"
#include "exceptions/steam_workshop_directory_not_found.hpp"

#include "static_todo.hpp"

namespace doctest
{
    template<> struct StringMaker<std::vector<std::string>>
    {
        static String convert(std::vector<std::string> const &vec)
        {
            std::string output  = "vector<string>: {";
            for (auto const &str : vec)
                output += fmt::format("\"{}\", ", str);
            output += "}\n";
            return output.c_str();
        }
    };
    template<> struct StringMaker<std::vector<std::filesystem::path>>
    {
        static String convert(std::vector<std::filesystem::path> const &vec)
        {
            std::string output  = "vector<path>: {";
            for (auto const &str : vec)
                output += fmt::format("\"{}\", ", str.string());
            output += '\n';
            return output.c_str();
        }
    };
}

using trompeloeil::_;

class SteamUtilsTests
{
    public:
        SteamUtilsTests()
        {
            spdlog::set_level(spdlog::level::trace);
        }

        FilesystemUtilsMock filesystemUtilsMock;
        StdUtilsMock stdUtilsMock;
        VdfMock vdfMock;

        std::filesystem::path const default_steam_path = "/somewhere/steam";
        std::filesystem::path const default_config_path = default_steam_path / "config/config.vdf";
        std::filesystem::path const default_library_folders_path = default_steam_path / "config/libraryfolders.vdf";

        std::string const empty_file_content = "";
        std::string const arma3_workshop_id = "107410";

        std::string_view const vdf_config = R"vdf(
"libraryfolders"
{
    "0"
    {
        "path" "/somewhere/steam"
    }
    "1"
    {
        "path" "/mnt/games/SteamLibrary"
    }
    "2"
    {
        "path" "/mnt/disk2/steamgames"
    }
}
)vdf";
        std::vector<std::string> const steam_library_dirs{"/mnt/games/SteamLibrary", "/mnt/disk2/steamgames"};
};

TEST_CASE_FIXTURE(SteamUtilsTests, "Constructor_Success")
{
    GIVEN("Steam config file that exists in correct path")
    {
        REQUIRE_CALL(filesystemUtilsMock, Exists(default_config_path)).RETURN(true);
        REQUIRE_CALL(filesystemUtilsMock, RealPath(default_steam_path)).RETURN(default_steam_path);

        WHEN("SteamUtils are constructed"){
            THEN("Exception is not thrown"){
                CHECK_NOTHROW(SteamUtils steam({default_steam_path}));}}
    }
}


TEST_CASE_FIXTURE(SteamUtilsTests, "Constructor_Failed_InvalidPaths")
{
    GIVEN("Not existing Steam config file")
    {
        std::filesystem::path const non_existent_path = "/nowhere";
        std::filesystem::path const non_existent_config = non_existent_path / "config/config.vdf";

        REQUIRE_CALL(filesystemUtilsMock, Exists(non_existent_config)).RETURN(false);
        WHEN("SteamUtils are constructed")
        {
            THEN("Exception is thrown")
            {
                CHECK_THROWS_AS(SteamUtils(std::vector<std::filesystem::path>{non_existent_path}), SteamInstallNotFoundException);
            }
        }
    }
}

TEST_CASE_FIXTURE(SteamUtilsTests, "FindInstallPaths_Success")
{
    GIVEN("Valid Steam config file with SteamLibrary and two custom libraries")
    {
        std::vector<std::filesystem::path> expected_paths{default_steam_path, "/mnt/games/SteamLibrary", "/mnt/disk2/steamgames"};

        REQUIRE_CALL(filesystemUtilsMock, Exists(default_config_path)).RETURN(true);
        REQUIRE_CALL(filesystemUtilsMock, RealPath(default_steam_path)).RETURN(default_steam_path);
        REQUIRE_CALL(stdUtilsMock, FileReadAllText(default_library_folders_path)).LR_RETURN(empty_file_content);
        REQUIRE_CALL(vdfMock, LoadFromText(empty_file_content, false, _));
        REQUIRE_CALL(vdfMock, GetValuesWithFilter("path", _)).LR_RETURN(steam_library_dirs);

        SteamUtils steam({default_steam_path});
        WHEN("GetInstallPaths is called")
        {
            THEN("Main SteamLibrary and two custom libraries are returned")
            {
                CHECK_EQ(expected_paths, steam.GetInstallPaths());
            }
        }
    }
}

TEST_CASE_FIXTURE(SteamUtilsTests, "GetGamePathFromInstallPath_Success")
{
    GIVEN("Valid appmanifest for Arma 3 game")
    {
        std::filesystem::path const arma3_path = default_steam_path / "steamapps/common/Arma 3";
        std::filesystem::path const arma3_manifest_path = default_steam_path / "steamapps/appmanifest_107410.acf";

        REQUIRE_CALL(filesystemUtilsMock, Exists(default_config_path)).RETURN(true);
        REQUIRE_CALL(filesystemUtilsMock, RealPath(default_steam_path)).RETURN(default_steam_path);
        REQUIRE_CALL(stdUtilsMock, FileReadAllText(arma3_manifest_path)).LR_RETURN(empty_file_content);
        REQUIRE_CALL(vdfMock, LoadFromText(empty_file_content, false, _)).SIDE_EFFECT(_3.KeyValue["AppState/installdir"] = "Arma 3");

        SteamUtils steam({default_steam_path});
        WHEN("Getting install path of Arma 3")
        {
            THEN("Arma 3 path is returned")
            {
                CHECK_EQ(arma3_path, steam.GetGamePathFromInstallPath(default_steam_path, arma3_workshop_id));
            }
        }
    }
}

TEST_CASE_FIXTURE(SteamUtilsTests, "GetWorkshopDir_Success")
{
    GIVEN("Valid appid for installed game")
    {
        std::string const invalid_game = "107411";
        std::filesystem::path const expected_workshop_path = default_steam_path / "steamapps/workshop/content" / arma3_workshop_id;
        std::filesystem::path const not_existing_workshop_path = default_steam_path / "steamapps/workshop/content" / invalid_game;

        REQUIRE_CALL(filesystemUtilsMock, Exists(default_config_path)).RETURN(true);
        REQUIRE_CALL(filesystemUtilsMock, RealPath(default_steam_path)).RETURN(default_steam_path);
        REQUIRE_CALL(filesystemUtilsMock, Exists(expected_workshop_path)).RETURN(true);
        REQUIRE_CALL(filesystemUtilsMock, Exists(not_existing_workshop_path)).RETURN(false);

        SteamUtils steam({default_steam_path});

        WHEN("Getting workshop path for installed game")
        {
            THEN("Workshop path for installed game is returned")
            {
                CHECK_EQ(expected_workshop_path, steam.GetWorkshopPath(default_steam_path, arma3_workshop_id));
                CHECK_THROWS_AS(steam.GetWorkshopPath(default_steam_path, invalid_game), SteamWorkshopDirectoryNotFoundException);
            }
        }
    }
}

TEST_CASE_FIXTURE(SteamUtilsTests, "GetWorkshopDir_Failed_NotExistingApp")
{
    GIVEN("Valid appid for installed game")
    {
        std::string const invalid_game = "107411";
        std::filesystem::path const not_existing_workshop_path = default_steam_path / "steamapps/workshop/content" / invalid_game;

        REQUIRE_CALL(filesystemUtilsMock, Exists(default_config_path)).RETURN(true);
        REQUIRE_CALL(filesystemUtilsMock, RealPath(default_steam_path)).RETURN(default_steam_path);
        REQUIRE_CALL(filesystemUtilsMock, Exists(not_existing_workshop_path)).RETURN(false);

        SteamUtils steam({default_steam_path});

        WHEN("Getting workshop path for not installed game")
        {
            THEN("Exception is thrown")
            {
                CHECK_THROWS_AS(steam.GetWorkshopPath(default_steam_path, invalid_game), SteamWorkshopDirectoryNotFoundException);
            }
        }
    }
}

TEST_CASE_FIXTURE(SteamUtilsTests, "GetCompatibilityToolForAppId")
{
    GIVEN("App id to find compatibility tool for")
    {
        std::uint64_t const appid = 107410;

        REQUIRE_CALL(filesystemUtilsMock, Exists(default_config_path)).RETURN(true);
        REQUIRE_CALL(filesystemUtilsMock, RealPath(default_steam_path)).RETURN(default_steam_path);
        SteamUtils steam({default_steam_path});

        ALLOW_CALL(stdUtilsMock, FileReadAllText(default_config_path)).LR_RETURN(default_config_path);

        auto const steam_compatibility_tool_dir = default_steam_path / "compatibilitytools.d/proton_316";
        auto const system_compatibility_tool_dir = "/usr/share/steam/compatibilitytools.d/proton_316";
        auto const filter = fmt::format("CompatToolMapping/{}/name", appid);
        REQUIRE_CALL(vdfMock, LoadFromText(default_config_path.string(), false, _));

        WHEN("Config file does not contain key about compatibility tool for appid")
        {
            REQUIRE_CALL(vdfMock, GetValuesWithFilter(filter, _)).RETURN(std::vector<std::string>{});
            THEN("Exception is thrown")
            {
                CHECK_THROWS_AS(steam.GetCompatibilityToolForAppId(appid), std::runtime_error);
            }
        }

        WHEN("Config file contains key about compatibility tool for appid")
        {
            trompeloeil::sequence sequence;

            auto const compatibility_tool_shortname = "proton_316";
            auto const log_file_path = default_steam_path / "logs/compat_log.txt";

            REQUIRE_CALL(vdfMock, GetValuesWithFilter(filter, _)).RETURN(std::vector<std::string>{compatibility_tool_shortname});

            WHEN("Requested tool is user tool (exists in compatibilitytools.d)")
            {
                REQUIRE_CALL(filesystemUtilsMock, Exists(steam_compatibility_tool_dir)).RETURN(true);

                auto const manifest_file = steam_compatibility_tool_dir / "toolmanifest.vdf";
                auto const manifest_commandline_key = "manifest/commandline";
                auto const manifest_commandline = "/proton run";

                REQUIRE_CALL(stdUtilsMock, FileReadAllText(manifest_file)).LR_RETURN(manifest_file);

                WHEN("Manifest file does not contain required key")
                {
                    REQUIRE_CALL(vdfMock, LoadFromText(manifest_file.string(), false, _));

                    THEN("Exception is thrown")
                    {
                        CHECK_THROWS_AS(steam.GetCompatibilityToolForAppId(appid), std::runtime_error);
                    }
                }

                WHEN("Manifest file contains manifest/commandline key")
                {
                    REQUIRE_CALL(vdfMock, LoadFromText(manifest_file.string(), false, _)).SIDE_EFFECT(_3.KeyValue[manifest_commandline_key] = manifest_commandline);
                    THEN("Valid compatibility tool path & argument pair is returned")
                    {
                        auto const compatibility_tool = steam.GetCompatibilityToolForAppId(appid);
                        CHECK_EQ("/somewhere/steam/compatibilitytools.d/proton_316/proton", compatibility_tool.first);
                        CHECK_EQ("run", compatibility_tool.second);
                    }
                }
            }

            WHEN("Requested tool is Steam's tool")
            {
                REQUIRE_CALL(filesystemUtilsMock, Exists(steam_compatibility_tool_dir)).RETURN(false);
                REQUIRE_CALL(filesystemUtilsMock, Exists(system_compatibility_tool_dir)).RETURN(false);

                WHEN("Log file does not contain matching information")
                {
                    REQUIRE_CALL(stdUtilsMock, FileReadAllText(log_file_path)).LR_RETURN(empty_file_content);

                    THEN("Exception is thrown")
                    {
                        CHECK_THROWS_AS(steam.GetCompatibilityToolForAppId(appid), std::runtime_error);
                    }
                }

                WHEN("Log file contains information about tool provided by Steam (AppID != 0)")
                {
                    auto const log_content = R"log([2005-04-02 21:37:36] Registering tool proton_5, AppID 1245040
[2005-04-02 21:37:36] Registering tool proton_411, AppID 1113280
[2005-04-02 21:37:36] Registering tool proton_42, AppID 1054830
[2005-04-02 21:37:36] Registering tool proton_316, AppID 961940
[2005-04-02 21:37:36] Registering tool proton_37, AppID 858280)log";
                    auto const tool_appmanifest_file = default_steam_path / "steamapps/appmanifest_961940.acf";
                    auto const install_dir_key = "AppState/installdir";
                    auto const install_dir = "Proton 3.16";
                    auto const manifest_file = default_steam_path / "steamapps/common/Proton 3.16/toolmanifest.vdf";
                    auto const manifest_commandline_key = "manifest/commandline";
                    auto const manifest_commandline = "/proton run";

                    REQUIRE_CALL(stdUtilsMock, FileReadAllText(default_library_folders_path)).RETURN(default_library_folders_path);
                    REQUIRE_CALL(vdfMock, LoadFromText(default_library_folders_path.string(), false, _)).IN_SEQUENCE(sequence);
                    REQUIRE_CALL(stdUtilsMock, FileReadAllText(log_file_path)).LR_RETURN(log_content);
                    REQUIRE_CALL(vdfMock, GetValuesWithFilter(_, _)).RETURN(std::vector<std::string>{default_steam_path}).IN_SEQUENCE(sequence);
                    ALLOW_CALL(stdUtilsMock, FileReadAllText(tool_appmanifest_file)).LR_RETURN(tool_appmanifest_file);

                    WHEN("VDF tool appmanifest does not contain required key")
                    {
                        REQUIRE_CALL(vdfMock, LoadFromText(tool_appmanifest_file.string(), false, _)).TIMES(2);

                        THEN("Exception is thrown")
                        {
                            CHECK_THROWS_AS(steam.GetCompatibilityToolForAppId(appid), std::runtime_error);
                        }
                    }

                    WHEN("VDF tool appmanifest contains AppState/installdir key")
                    {
                        REQUIRE_CALL(vdfMock, LoadFromText(tool_appmanifest_file.string(), false, _)).SIDE_EFFECT(_3.KeyValue[install_dir_key] = install_dir);
                        REQUIRE_CALL(stdUtilsMock, FileReadAllText(manifest_file)).LR_RETURN(manifest_file);

                        WHEN("Manifest file does not contain required key")
                        {
                            REQUIRE_CALL(vdfMock, LoadFromText(manifest_file.string(), false, _));

                            THEN("Exception is thrown")
                            {
                                CHECK_THROWS_AS(steam.GetCompatibilityToolForAppId(appid), std::runtime_error);
                            }
                        }

                        WHEN("Manifest file contains manifest/commandline key")
                        {
                            REQUIRE_CALL(vdfMock, LoadFromText(manifest_file.string(), false, _)).SIDE_EFFECT(_3.KeyValue[manifest_commandline_key] = manifest_commandline);
                            THEN("Valid compatibility tool path & argument pair is returned")
                            {
                                auto const compatibility_tool = steam.GetCompatibilityToolForAppId(appid);
                                CHECK_EQ("/somewhere/steam/steamapps/common/Proton 3.16/proton", compatibility_tool.first);
                                CHECK_EQ("run", compatibility_tool.second);
                            }
                        }
                    }
                }
            }
        }
    }
}

TEST_CASE_FIXTURE(SteamUtilsTests, "GetInstallPathFromGamePath_Success")
{
    GIVEN("Game install path")
    {
        auto const game_path = default_steam_path / "steamapps/common/Arma 3";

        REQUIRE_CALL(filesystemUtilsMock, Exists(default_config_path)).RETURN(true);
        REQUIRE_CALL(filesystemUtilsMock, RealPath(default_steam_path)).RETURN(default_steam_path);
        SteamUtils steam({default_steam_path});

        WHEN("Obtaining Steam install path from game path")
        {
            THEN("Steam install path is returned")
            {
                auto returned_path = steam.GetInstallPathFromGamePath(game_path);

                // For some unknown reason 'weakly_canonical' likes to return with trailing slash
                if (StringUtils::EndsWith(returned_path.string(), "/"))
                    returned_path = returned_path.string().substr(0, returned_path.string().length() - 1);

                CHECK_EQ(default_steam_path, returned_path);
            }
        }
    }
}
