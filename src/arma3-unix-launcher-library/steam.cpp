#include "steam.hpp"

#include <exception>
#include <filesystem>
#include <stdexcept>

#include "string_utils.hpp"
#include "vdf.hpp"

#include "exceptions/steam_install_not_found.hpp"
#include "exceptions/steam_workshop_directory_not_found.hpp"

using namespace StringUtils;

Steam::Steam(std::vector<std::string> search_paths)
{
    steam_path_ = "";
    for (std::string &path : search_paths)
    {
        std::string replace_var = Replace(path, "$HOME", getenv("HOME"));
        std::string final_path = replace_var + config_path_;
        if (std::filesystem::exists(final_path))
        {
            steam_path_ = replace_var;
            break;
        }
    }
    if (steam_path_.empty())
        throw SteamInstallNotFoundException();
}

const std::string &Steam::GetSteamPath() noexcept
{
    return steam_path_;
}

std::vector<std::string> Steam::GetInstallPaths()
{
    std::vector<std::string> ret;
    ret.emplace_back(steam_path_);

    VDF vdf;
    vdf.LoadFromFile(steam_path_ + config_path_);

    for (const auto &key : vdf.GetValuesWithFilter("BaseInstallFolder"))
        ret.push_back(key);

    return ret;
}

std::string Steam::GetWorkshopPath(std::string appid)
{
    auto install_paths = GetInstallPaths();
    install_paths.emplace_back(steam_path_);

    for (const auto &path : install_paths)
    {
        std::string proposed_path = path + "/steamapps/workshop/content/" + appid;
        if (std::filesystem::exists(proposed_path))
            return proposed_path;
    }

    throw SteamWorkshopDirectoryNotFoundException(appid);
}

#ifndef DOCTEST_CONFIG_DISABLE
//GCOV_EXCL_START
#include <doctest.h>
#include "tests.hpp"

class SteamTests
{
    public:
        std::string dir = Tests::Utils::GetWorkDir() + "/test-files";
};

TEST_CASE_FIXTURE(SteamTests, "FindInstallPaths")
{
    Steam steam({dir + "/steam"});
    std::vector<std::string> paths { dir + "/steam", "/mnt/games/SteamLibrary", "/mnt/disk2/steamgames" };
    CHECK_EQ(paths, steam.GetInstallPaths());
}

TEST_CASE_FIXTURE(SteamTests, "InvalidPaths")
{
    CHECK_THROWS_AS(Steam(std::vector<std::string> {"/nowhere"}), SteamInstallNotFoundException);
}

TEST_CASE_FIXTURE(SteamTests, "GetSteamPath")
{
    Steam steam({dir + "/steam"});
    CHECK_EQ(dir + "/steam", steam.GetSteamPath());
}

TEST_CASE_FIXTURE(SteamTests, "GetWorkshopDir")
{
    Steam steam({dir + "/steam"});
    CHECK_EQ(dir + "/steam/steamapps/workshop/content/107410", steam.GetWorkshopPath("107410"));
}

//GCOV_EXCL_STOP
#endif
