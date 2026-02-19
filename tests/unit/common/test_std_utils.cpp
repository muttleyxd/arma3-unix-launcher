#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <fmt/format.h>

#include "default_test_reporter.hpp"
#include "std_utils.hpp"

TEST_CASE("GetConfigFilePath")
{
    using namespace StdUtils;

    GIVEN("Filename to get config path for")
    {
        std::filesystem::path config_file = "a3unixlauncher.cfg";

        constexpr char const *app_name = "a3unixlauncher";

        WHEN("XDG_CONFIG_HOME is not set")
        {
            REQUIRE_EQ(0, unsetenv("XDG_CONFIG_HOME"));

            auto path = GetConfigFilePath(config_file, app_name);

            THEN("$HOME/.config is used")
            CHECK_EQ(fmt::format("{}/.config/a3unixlauncher/{}", getenv("HOME"), config_file.string()), path);
        }

        WHEN("XDG_CONFIG_HOME is set")
        {
            std::filesystem::path xdg_config_home = "/configs";
            REQUIRE_EQ(0, setenv("XDG_CONFIG_HOME", xdg_config_home.string().c_str(), 1));

            auto path = GetConfigFilePath(config_file, app_name);

            THEN("XDG_CONFIG_HOME is used")
            CHECK_EQ(fmt::format("{}/a3unixlauncher/{}", xdg_config_home.string(), config_file.string()), path);
        }
    }
}
