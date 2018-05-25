#include "gtest/gtest.h"

#include "filesystem.hpp"
#include "setup.hpp"
#include "steam.hpp"

#include <exception>
#include <string>
#include <vector>

class SteamTests : public ::testing::Test
{
    public:
        std::string dir = GetWorkDir() + "/test-files";
};

TEST_F(SteamTests, FindInstallPaths)
{
    Steam steam(std::vector<std::string> { dir + "/steam"});
    std::vector<std::string> paths { "/mnt/games/SteamLibrary", "/mnt/disk2/steamgames" };
    ASSERT_EQ(steam.GetInstallPaths(), paths);
}

TEST_F(SteamTests, InvalidPaths)
{
    ASSERT_THROW(Steam steam(std::vector<std::string> { "/nowhere"}), std::invalid_argument);
}

TEST_F(SteamTests, GetSteamPath)
{
    Steam steam;
    std::string home_path = getenv("HOME");
    std::string steam_path = steam.GetSteamPath();
    if (steam_path != home_path + "/.steam/steam" && steam_path != home_path + "/.local/share/Steam")
    {
        EXPECT_EQ(home_path + "/.steam", steam.GetSteamPath());
        EXPECT_EQ(home_path + "/.local/share/Steam", steam.GetSteamPath());
    }
}
