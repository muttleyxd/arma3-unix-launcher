#include "steam.hpp"

#include <exception>
#include <stdexcept>

#include "string_utils.hpp"
#include "vdf.hpp"

#include "exceptions/steam_install_not_found.hpp"
#include "exceptions/steam_workshop_directory_not_found.hpp"

using namespace StringUtils;

Steam::Steam(std::vector<std::filesystem::path> search_paths)
{
    steam_path_ = "";
    for (const auto &path : search_paths)
    {
        std::filesystem::path replace_var = Replace(path.c_str(), "$HOME", getenv("HOME"));
        std::string final_path = replace_var / config_path_;
        if (std::filesystem::exists(final_path))
        {
            steam_path_ = replace_var;
            break;
        }
    }
    if (steam_path_.empty())
        throw SteamInstallNotFoundException();
}

const std::filesystem::path &Steam::GetSteamPath() noexcept
{
    return steam_path_;
}

std::vector<std::filesystem::path> Steam::GetInstallPaths()
{
    std::vector<std::filesystem::path> ret;
    ret.emplace_back(steam_path_);

    VDF vdf;
    vdf.LoadFromFile(steam_path_ / config_path_);

    for (const auto &key : vdf.GetValuesWithFilter("BaseInstallFolder"))
        ret.emplace_back(key);

    return ret;
}

std::filesystem::path Steam::GetWorkshopPath(std::string const &appid)
{
    auto install_paths = GetInstallPaths();
    install_paths.emplace_back(steam_path_);

    for (const auto &path : install_paths)
    {
        std::filesystem::path proposed_path = path / "steamapps/workshop/content" / appid;
        if (std::filesystem::exists(proposed_path))
            return proposed_path;
    }

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
    std::vector<std::filesystem::path> paths { test_files_path / "steam", "/mnt/games/SteamLibrary", "/mnt/disk2/steamgames" };
    CHECK_EQ(paths, steam.GetInstallPaths());
}

TEST_CASE_FIXTURE(Tests::Fixture, "InvalidPaths")
{
    CHECK_THROWS_AS(Steam(std::vector<std::filesystem::path> {"/nowhere"}), SteamInstallNotFoundException);
}

TEST_CASE_FIXTURE(Tests::Fixture, "GetSteamPath")
{
    Steam steam({test_files_path / "steam"});
    CHECK_EQ(test_files_path / "steam", steam.GetSteamPath());
}

TEST_CASE_FIXTURE(Tests::Fixture, "GetWorkshopDir")
{
    Steam steam({test_files_path / "steam"});
    CHECK_EQ(test_files_path / "steam/steamapps/workshop/content/107410", steam.GetWorkshopPath("107410"));
}

TEST_SUITE_END();

//GCOV_EXCL_STOP
#endif
