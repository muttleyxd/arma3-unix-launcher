#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <fmt/format.h>

#include "default_test_reporter.hpp"

#include "std_utils.hpp"

TEST_CASE("Contains")
{
    using namespace StdUtils;

    SUBCASE("integers")
    {
        std::vector<int> numbers{1, 2, 4, 5, 6};
        CHECK(Contains(numbers, 1));
        CHECK(Contains(numbers, 2));
        CHECK_FALSE(Contains(numbers, 3));
    }

    SUBCASE("strings")
    {
        std::vector<std::string> strings{"test", "123", "addons"};
        CHECK(Contains(strings, "test"));
        CHECK(Contains(strings, "addons"));
        CHECK_FALSE(Contains(strings, "ADDONS"));
    }
}

TEST_CASE("ContainsKey")
{
    using namespace StdUtils;

    SUBCASE("int, int")
    {
        std::map<int, int> numbers{{0, 1}, {1, 1}, {3, 3}};
        CHECK(ContainsKey(numbers, 0));
        CHECK(ContainsKey(numbers, 1));
        CHECK_FALSE(ContainsKey(numbers, 2));
    }

    SUBCASE("string, string")
    {
        std::map<std::string, std::string> strings{{"test", "testValue"}, {"testKey", "testValue2"}};
        CHECK(ContainsKey(strings, "test"));
        CHECK(ContainsKey(strings, "testKey"));
        CHECK_FALSE(ContainsKey(strings, "nothing here"));
    }
}

TEST_CASE("GetConfigFilePath")
{
    using namespace StdUtils;

    GIVEN("Filename to get config path for")
    {
        std::filesystem::path config_file = "a3unixlauncher.cfg";

        WHEN("XDG_CONFIG_HOME is not set")
        {
            REQUIRE_EQ(0, unsetenv("XDG_CONFIG_HOME"));

            auto path = GetConfigFilePath(config_file);

            THEN("$HOME/.config is used")
            CHECK_EQ(fmt::format("{}/.config/a3unixlauncher/{}", getenv("HOME"), config_file.string()), path);
        }

        WHEN("XDG_CONFIG_HOME is set")
        {
            std::filesystem::path xdg_config_home = "/configs";
            REQUIRE_EQ(0, setenv("XDG_CONFIG_HOME", xdg_config_home.string().c_str(), 1));

            auto path = GetConfigFilePath(config_file);

            THEN("XDG_CONFIG_HOME is used")
            CHECK_EQ(fmt::format("{}/a3unixlauncher/{}", xdg_config_home.string(), config_file.string()), path);
        }
    }
}
