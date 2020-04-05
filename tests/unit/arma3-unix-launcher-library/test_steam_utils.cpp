#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include <trompeloeil.hpp>
#include "default_test_reporter.hpp"

#include <filesystem>

#include <fmt/format.h>

#include "steam_utils.hpp"

#include "filesystem_utils.hpp"
#include "std_utils.hpp"
#include "vdf.hpp"
#include "mock/filesystem_utils.hpp"
#include "mock/std_utils.hpp"
#include "mock/vdf.hpp"

#include "exceptions/steam_install_not_found.hpp"
#include "exceptions/steam_workshop_directory_not_found.hpp"

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
        FilesystemUtilsMock filesystemUtilsMock;
        StdUtilsMock stdUtilsMock;
        VdfMock vdfMock;

        std::filesystem::path const default_steam_path = "/somewhere/steam";
        std::filesystem::path const default_config_path = default_steam_path / "config/config.vdf";

        std::string const empty_file_content = "";
        std::string const arma3_workshop_id = "107410";

        std::string_view const vdf_config = R"vdf(
"InstallConfigStore"
{
	"Software"
	{
		"Valve"
		{
			"Steam"
			{
				"BaseInstallFolder_1"		"/mnt/games/SteamLibrary"
				"BaseInstallFolder_2"		"/mnt/disk2/steamgames"
			}
		}
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

        WHEN("SteamUtils are constructed")
        THEN("Exception is not thrown")
        CHECK_NOTHROW(SteamUtils steam({default_steam_path}));
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
        THEN("Exception is thrown")
        CHECK_THROWS_AS(SteamUtils(std::vector<std::filesystem::path>{non_existent_path}), SteamInstallNotFoundException);
    }
}

TEST_CASE_FIXTURE(SteamUtilsTests, "FindInstallPaths_Success_WithoutCustomLibraries")
{
    GIVEN("Valid Steam config file not containing any custom libraries [game install directories]")
    {
        std::vector<std::filesystem::path> expected_paths{default_steam_path};

        REQUIRE_CALL(filesystemUtilsMock, Exists(default_config_path)).RETURN(true);
        REQUIRE_CALL(stdUtilsMock, FileReadAllText(default_config_path)).LR_RETURN(empty_file_content);
        REQUIRE_CALL(vdfMock, LoadFromText(empty_file_content, false, _));
        REQUIRE_CALL(vdfMock, GetValuesWithFilter("BaseInstallFolder", _)).RETURN(std::vector<std::string>{});

        SteamUtils steam({default_steam_path});
        WHEN("GetInstallPaths is called")
        THEN("Only main SteamLibrary is returned")
        CHECK_EQ(expected_paths, steam.GetInstallPaths());
    }
}

TEST_CASE_FIXTURE(SteamUtilsTests, "FindInstallPaths_Success_WithCustomLibraries")
{
    GIVEN("Valid Steam config file with two custom libraries")
    {
        std::vector<std::filesystem::path> expected_paths{default_steam_path, "/mnt/games/SteamLibrary", "/mnt/disk2/steamgames"};

        REQUIRE_CALL(filesystemUtilsMock, Exists(default_config_path)).RETURN(true);
        REQUIRE_CALL(stdUtilsMock, FileReadAllText(default_config_path)).LR_RETURN(empty_file_content);
        REQUIRE_CALL(vdfMock, LoadFromText(empty_file_content, false, _));
        REQUIRE_CALL(vdfMock, GetValuesWithFilter("BaseInstallFolder", _)).LR_RETURN(steam_library_dirs);

        SteamUtils steam({default_steam_path});
        WHEN("GetInstallPaths is called")
        THEN("Main SteamLibrary and two custom libraries are returned")
        CHECK_EQ(expected_paths, steam.GetInstallPaths());
    }
}

TEST_CASE_FIXTURE(SteamUtilsTests, "GetGamePathFromInstallPath_Success")
{
    GIVEN("Valid appmanifest for Arma 3 game")
    {
        std::filesystem::path const arma3_path = default_steam_path / "steamapps/common/Arma 3";
        std::filesystem::path const arma3_manifest_path = default_steam_path / "steamapps/appmanifest_107410.acf";

        REQUIRE_CALL(filesystemUtilsMock, Exists(default_config_path)).RETURN(true);
        REQUIRE_CALL(stdUtilsMock, FileReadAllText(arma3_manifest_path)).LR_RETURN(empty_file_content);
        REQUIRE_CALL(vdfMock, LoadFromText(empty_file_content, false, _)).SIDE_EFFECT(_3.KeyValue["AppState/installdir"] = "Arma 3");

        SteamUtils steam({default_steam_path});
        WHEN("Getting install path of Arma 3")
        THEN("Arma 3 path is returned")
        CHECK_EQ(arma3_path, steam.GetGamePathFromInstallPath(default_steam_path, arma3_workshop_id));
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
        REQUIRE_CALL(filesystemUtilsMock, Exists(expected_workshop_path)).RETURN(true);
        REQUIRE_CALL(filesystemUtilsMock, Exists(not_existing_workshop_path)).RETURN(false);

        SteamUtils steam({default_steam_path});

        WHEN("Getting workshop path for installed game")
        THEN("Workshop path for installed game is returned")
        CHECK_EQ(expected_workshop_path, steam.GetWorkshopPath(default_steam_path, arma3_workshop_id));
        CHECK_THROWS_AS(steam.GetWorkshopPath(default_steam_path, invalid_game), SteamWorkshopDirectoryNotFoundException);
    }
}

TEST_CASE_FIXTURE(SteamUtilsTests, "GetWorkshopDir_Failed_NotExistingApp")
{
    GIVEN("Valid appid for installed game")
    {
        std::string const invalid_game = "107411";
        std::filesystem::path const not_existing_workshop_path = default_steam_path / "steamapps/workshop/content" / invalid_game;

        REQUIRE_CALL(filesystemUtilsMock, Exists(default_config_path)).RETURN(true);
        REQUIRE_CALL(filesystemUtilsMock, Exists(not_existing_workshop_path)).RETURN(false);

        SteamUtils steam({default_steam_path});

        WHEN("Getting workshop path for not installed game")
        THEN("Exception is thrown")
        CHECK_THROWS_AS(steam.GetWorkshopPath(default_steam_path, invalid_game), SteamWorkshopDirectoryNotFoundException);
    }
}
