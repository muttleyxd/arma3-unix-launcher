#include "gtest/gtest.h"

#include "filesystem.hpp"
#include "setup.hpp"

#include "mod.hpp"
#include "steam.hpp"

#include "exceptions/steam_install_not_found.hpp"

#include <exception>
#include <string>
#include <vector>

class GetModsTests : public ::testing::Test
{
    public:
        std::string dir = GetWorkDir() + "/test-files";
};

TEST_F(GetModsTests, GetMods)
{
    Steam steam({dir + "/steam"});
    std::vector<std::string> paths { dir + "/steam", "/mnt/games/SteamLibrary", "/mnt/disk2/steamgames" };
    ASSERT_EQ(steam.GetInstallPaths(), paths);
    //ASSERT_NO_THROW
}
