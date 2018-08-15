#include "gtest/gtest.h"

#include "filesystem.hpp"
#include "setup.hpp"

#define private public
#define protected public
#include "vdf.hpp"
#undef private
#undef protected

class VdfTests : public ::testing::Test
{
    public:
        std::string dir = GetWorkDir() + "/test-files";
};

TEST_F(VdfTests, BasicFilter)
{
    ASSERT_NO_THROW(
    {
        VDF vdf;
        std::string key = "Key";
        std::string value = "Value";
        for (int i = 0; i < 10; i++)
        {
            std::string number = std::to_string(i + 1);
            vdf.KeyValue[key + number] = value + number;
        }
        ASSERT_EQ(vdf.GetValuesWithFilter("This should be empty").size(), 0);
        ASSERT_EQ(vdf.GetValuesWithFilter("Key").size(), vdf.KeyValue.size());

        ASSERT_EQ(vdf.GetValuesWithFilter("Key1").size(), 2);
    });
}

TEST_F(VdfTests, BasicParser)
{
    ASSERT_NO_THROW((
    {
        VDF vdf;
        std::string simple_key_value = "\"Key\"\"Value\"";
        vdf.ParseVDF(simple_key_value);
        ASSERT_EQ("Value", vdf.KeyValue["Key"]);
        ASSERT_EQ(1, vdf.KeyValue.size());
    }));

    ASSERT_NO_THROW((
    {
        VDF vdf;
        std::string simple_key_value = "\"Branch\"{\"Key\"\"Value\"}";
        vdf.ParseVDF(simple_key_value);
        ASSERT_EQ("Value", vdf.KeyValue["Branch/Key"]);
        ASSERT_EQ(1, vdf.KeyValue.size());
    }));
}

TEST_F(VdfTests, LoadFromFile)
{
    ASSERT_NO_THROW((
    {
        VDF vdf, vdfWithTabs;
        vdf.LoadFromFile(dir + "/vdf-valid.vdf");
        vdfWithTabs.LoadFromFile(dir + "/vdf-valid-mixed-spaces-with-tabs.vdf");
        ASSERT_EQ(vdf.KeyValue, vdfWithTabs.KeyValue);
        ASSERT_EQ(8, vdf.KeyValue.size());
    }));
}

TEST_F(VdfTests, ParserThenFilter)
{
    ASSERT_NO_THROW((
    {
        VDF vdf;
        vdf.LoadFromFile(dir + "/vdf-valid.vdf");
        std::vector<std::string> filtered = vdf.GetValuesWithFilter("BaseInstallFolder");
        std::vector<std::string> paths{"/mnt/games/SteamLibrary", "/home/user/SteamLibrary", "/run/media/user/SteamLibrary", "/somerandompath/steamlibrary"};
        std::sort(filtered.begin(), filtered.end());
        std::sort(paths.begin(), paths.end());
        ASSERT_EQ(filtered, paths);
    }));
}

