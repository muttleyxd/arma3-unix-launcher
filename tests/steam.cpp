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
    try
    {
        Steam steam(std::vector<std::string> { "/nowhere"});
        ASSERT_FALSE(true) << "This should not be executed - exception should be thrown earlier";
    }
    catch (std::invalid_argument exception)
    {
        ASSERT_EQ(exception.what(), "Steam::Steam() - Cannot find Steam install");
    }
}
