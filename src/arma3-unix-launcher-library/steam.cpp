#include "steam.hpp"

#include <exception>
#include <stdexcept>

#include "string_utils.hpp"
#include "vdf.hpp"

#include "exceptions/steam_install_not_found.hpp"
#include "exceptions/steam_workshop_directory_not_found.hpp"

using namespace StringUtils;

using std::filesystem::exists;
using std::filesystem::path;

Steam::Steam(std::vector<path> search_paths)
{
    steam_path_ = "";
    for (const auto &search_path : search_paths)
    {
        path replace_var = Replace(search_path.c_str(), "$HOME", getenv("HOME"));
        std::string final_path = replace_var / config_path_;
        if (exists(final_path))
        {
            steam_path_ = replace_var;
            break;
        }
    }
    if (steam_path_.empty())
        throw SteamInstallNotFoundException();
}

path const &Steam::GetSteamPath() const noexcept
{
    return steam_path_;
}

std::vector<path> Steam::GetInstallPaths() const
{
    std::vector<std::filesystem::path> ret;
    ret.emplace_back(steam_path_);

    VDF vdf;
    vdf.LoadFromFile(steam_path_ / config_path_);

    for (const auto &key : vdf.GetValuesWithFilter("BaseInstallFolder"))
        ret.emplace_back(key);

    return ret;
}

path Steam::GetGamePathFromInstallPath(path const &install_path, std::string const &appid) const
{
    std::filesystem::path manifest_file = install_path / "steamapps" / ("appmanifest_" + appid + ".acf");

    VDF vdf;
    vdf.LoadFromFile(manifest_file);
    return install_path / "steamapps/common" / vdf.KeyValue["AppState/installdir"];;
}

path Steam::GetWorkshopPath(path const &install_path, std::string const &appid) const
{
    path proposed_path = install_path / "steamapps/workshop/content" / appid;
    if (exists(proposed_path))
        return proposed_path;

    throw SteamWorkshopDirectoryNotFoundException(appid);
}

#ifndef DOCTEST_CONFIG_DISABLE
//GCOV_EXCL_START
#include <doctest.h>
#include "tests.hpp"

TEST_SUITE_BEGIN("Steam");

TEST_CASE_FIXTURE(Tests::Fixture, "FindInstallPaths")
{
    Steam steam({test_files_path / "steam"});
    std::vector<path> paths { test_files_path / "steam", "/mnt/games/SteamLibrary", "/mnt/disk2/steamgames" };
    CHECK_EQ(paths, steam.GetInstallPaths());
}

TEST_CASE_FIXTURE(Tests::Fixture, "InvalidPaths")
{
    CHECK_THROWS_AS(Steam(std::vector<path> {"/nowhere"}), SteamInstallNotFoundException);
}

TEST_CASE_FIXTURE(Tests::Fixture, "GetSteamPath")
{
    Steam steam({test_files_path / "steam"});
    CHECK_EQ(test_files_path / "steam", steam.GetSteamPath());
}

TEST_CASE_FIXTURE(Tests::Fixture, "GetGamePathFromInstallPath")
{
    Steam steam({test_files_path / "steam"});
    CHECK_EQ(test_files_path / "steam/steamapps/common/Arma 3", steam.GetGamePathFromInstallPath(steam.GetInstallPaths()[0],
             "107410"));
}

TEST_CASE_FIXTURE(Tests::Fixture, "GetWorkshopDir")
{
    Steam steam({test_files_path / "steam"});
    std::string workshop_path = test_files_path / "steam/steamapps/workshop/content/107410";

    CHECK_EQ(workshop_path, steam.GetWorkshopPath(steam.GetInstallPaths()[0], "107410"));

    CHECK_THROWS_AS(steam.GetWorkshopPath(steam.GetInstallPaths()[0], "107411"), SteamWorkshopDirectoryNotFoundException);
}

TEST_SUITE_END();

//GCOV_EXCL_STOP
#endif
