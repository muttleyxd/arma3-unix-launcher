#include "gtest/gtest.h"

#include "setup.hpp"

#include "steam.hpp"

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
    ASSERT_EQ(paths, steam.GetInstallPaths());
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

void im_retarded()
{
    char *ptr = (char*)42;
    *ptr = 42;
}

int main()
{
    im_retarded();
    return 5;
}
