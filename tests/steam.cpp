#include "gtest/gtest.h"

#include "filesystem.hpp"
#include "setup.hpp"

#define private public
#define protected public
#include "steam.hpp"
#undef private
#undef protected

#include <exception>
#include <string>
#include <vector>

#include <exceptions/steam_install_not_found.hpp>

class SteamTests : public ::testing::Test
{
    public:
        std::string dir = GetWorkDir() + "/test-files";
};

TEST_F(SteamTests, FindInstallPaths)
{
    Steam steam({dir + "/steam"});
    std::vector<std::string> paths { dir + "/steam", "/mnt/games/SteamLibrary", "/mnt/disk2/steamgames" };
    ASSERT_EQ(steam.GetInstallPaths(), paths);
}

TEST_F(SteamTests, InvalidPaths)
{
    ASSERT_THROW(Steam steam(std::vector<std::string> {"/nowhere"}), SteamInstallNotFoundException);
}

TEST_F(SteamTests, GetSteamPath)
{
    Steam steam({dir + "/steam"});
    ASSERT_EQ(dir + "/steam", steam.GetSteamPath());
}

TEST_F(SteamTests, GetWorkshopDir)
{
    Steam steam({dir + "/steam"});
    ASSERT_NO_THROW(ASSERT_EQ(dir + "/steam/steamapps/workshop/content/107410", steam.GetWorkshopPath("107410")));
}
