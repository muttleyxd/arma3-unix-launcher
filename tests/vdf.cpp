#include "gtest/gtest.h"

#include "filesystem.hpp"
#include "setup.hpp"
#include "vdf.hpp"

class VdfTests : public ::testing::Test
{
public:
    std::string dir = GetWorkDir() + "/test-files";
};

TEST_F(VdfTests, BasicFilter)
{
    VDF vdf;
    std::string key = "Key", value = "Value";
    for (int i = 0; i < 10; i++)
    {
        std::string number = std::to_string(i + 1);
        vdf.KeyValue[key + number] = value + number;
    }
    ASSERT_EQ(vdf.GetValuesWithFilter("This should be empty").size(), 0);
    ASSERT_EQ(vdf.GetValuesWithFilter("Key").size(), vdf.KeyValue.size());

    ASSERT_EQ(vdf.GetValuesWithFilter("Key1").size(), 2);
}

TEST_F(VdfTests, BasicParser)
{
    VDF vdf, vdfWithTabs;
    ASSERT_EQ(vdf.LoadFromFile(dir + "/vdf-valid.vdf"), 0);
    ASSERT_EQ(vdfWithTabs.LoadFromFile(dir + "/vdf-valid-mixed-spaces-with-tabs.vdf"), 0);
    ASSERT_EQ(vdf.KeyValue, vdfWithTabs.KeyValue);
}

TEST_F(VdfTests, ParserThenFilter)
{
    VDF vdf;
    ASSERT_EQ(vdf.LoadFromFile(dir + "/vdf-valid.vdf"), 0);
    std::vector<std::string> filtered = vdf.GetValuesWithFilter("BaseInstallFolder");
    std::vector<std::string> paths{"/mnt/games/SteamLibrary", "/home/user/SteamLibrary", "/run/media/user/SteamLibrary", "/somerandompath/steamlibrary"};
    std::sort(filtered.begin(), filtered.end());
    std::sort(paths.begin(), paths.end());
    ASSERT_EQ(filtered, paths);
}
