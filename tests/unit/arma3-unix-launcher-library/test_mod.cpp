#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include <trompeloeil.hpp>
#include "default_test_reporter.hpp"

#include "mod.hpp"
#include "exceptions/directory_not_found.hpp"

#include "filesystem_utils.hpp"
#include "std_utils.hpp"
#include "vdf.hpp"
#include "mock/filesystem_utils.hpp"
#include "mock/std_utils.hpp"
#include "mock/vdf.hpp"

std::ostream &operator<< (std::ostream &os, std::map<std::string, std::string> const &key_value)
{
    os << "\nMap begin:\n";
    for (auto const& [key, value] : key_value)
        os << key << ':' << value << '\n';
    os << "Map end:\n\n";
    return os;
}

namespace doctest
{
    template<> struct StringMaker<std::map<std::string, std::string>>
    {
        static String convert(std::map<std::string, std::string> const &map)
        {
            std::string output  = "map<string, string>: {\n";
            for (auto const& [key, value] : map)
                output += fmt::format("{}: {}\n", key, value);
            output += "}\n";
            return output.c_str();
        }
    };
}

using trompeloeil::_;

class ModTests
{
    public:
        FilesystemUtilsMock filesystemUtilsMock;
        StdUtilsMock stdUtilsMock;
        VdfMock vdfMock;

        std::filesystem::path const remove_stamina_path = "/remove_stamina";
        std::filesystem::path const remove_stamina_mod_cpp_path = remove_stamina_path / "mod.cpp";
        std::string const remove_stamina_mod_cpp = R"cpp(name = "Remove Stamina";
dir = "@Remove stamina";
picture = "logo.paa";
hidePicture = "false";
hideName = "false";
logo = "logo.paa";
description = "Simple mod which removes stamina from ArmA 3";
author = "Muttley";)cpp";
        std::string const remove_stamina_mod_cpp_missing_quotes = R"cpp(name = "Remove Stamina";
dir = "@Remove stamina";
picture = "logo.paa";
hidePicture = false;
hideName = false;
logo = "logo.paa";
description = "Simple mod which removes stamina from ArmA 3";
author = Muttley;
)cpp";
        std::string const remove_stamina_mod_cpp_no_whitespaces= R"cpp(name="Remove Stamina";dir="@Remove stamina";picture="logo.paa";hidePicture="false";hideName="false";logo="logo.paa";description="Simple mod which removes stamina from ArmA 3";author="Muttley";)cpp";
        std::map<std::string, std::string> const remove_stamina_map
        {
            {"name", "Remove Stamina"},
            {"dir", "@Remove stamina"},
            {"picture", "logo.paa"},
            {"hidePicture", "false"},
            {"hideName", "false"},
            {"logo", "logo.paa"},
            {"description", "Simple mod which removes stamina from ArmA 3"},
            {"author", "Muttley"},
            {"publishedid", "remove_stamina"}
        };
};

TEST_CASE_FIXTURE(ModTests, "Constructor_Success")
{
    GIVEN("Mod directory containing 'addons' subdirectory")
    {
        REQUIRE_CALL(filesystemUtilsMock, Ls(remove_stamina_path, _)).TIMES(2).RETURN(std::vector<std::string> {"addons"});

        WHEN("Subaddons directory exists")
        THEN("Mod is constructed, no exception thrown")
        Mod mod(remove_stamina_path);
    }
}

TEST_CASE_FIXTURE(ModTests, "Constructor_Failed_MissingAddonsDirectory")
{
    GIVEN("Mod path not containing 'addons' directory")
    {
        REQUIRE_CALL(filesystemUtilsMock, Ls(remove_stamina_path, true)).RETURN(std::vector<std::string> {});

        WHEN("./addons directory does not exist")
        THEN("Exception is thrown")
        CHECK_THROWS_AS(Mod(std::ref(remove_stamina_path)), DirectoryNotFoundException); //doctest weirdness
    }
}

TEST_CASE_FIXTURE(ModTests, "BasicParsing_Success")
{
    GIVEN("Mod path containing valid 'addons' and 'mod.cpp'")
    {
        REQUIRE_CALL(filesystemUtilsMock, Ls(remove_stamina_path, _)).TIMES(2).RETURN(std::vector<std::string> {"addons", "mod.cpp"});
        REQUIRE_CALL(stdUtilsMock, FileReadAllText(remove_stamina_mod_cpp_path)).LR_RETURN(remove_stamina_mod_cpp);

        WHEN("Mod is created")
        {
            Mod remove_stamina(remove_stamina_path);
            THEN("Appropriate keys and values are set")
            {
                CHECK_EQ(remove_stamina_map, remove_stamina.KeyValue);
            }
        }
    }
}

TEST_CASE_FIXTURE(ModTests, "MissingQuotes")
{
    GIVEN("Mod path containing valid 'addons' and 'mod.cpp' with missing quotes in one KeyValue pair")
    {
        REQUIRE_CALL(filesystemUtilsMock, Ls(remove_stamina_path, _)).TIMES(2).RETURN(std::vector<std::string> {"addons", "mod.cpp"});
        REQUIRE_CALL(stdUtilsMock, FileReadAllText(remove_stamina_mod_cpp_path)).LR_RETURN(
            remove_stamina_mod_cpp_missing_quotes);

        WHEN("Mod is created")
        {
            Mod remove_stamina(remove_stamina_path);
            THEN("Appropriate keys and values are set, despite missing quotes")
            {
                CHECK_EQ(remove_stamina_map, remove_stamina.KeyValue);
            }
        }
    }
}

TEST_CASE_FIXTURE(ModTests, "NoWhitespaces")
{
    GIVEN("Mod path containing valid 'addons' and 'mod.cpp' without whitespaces")
    {
        REQUIRE_CALL(filesystemUtilsMock, Ls(remove_stamina_path, _)).TIMES(2).RETURN(std::vector<std::string> {"addons", "mod.cpp"});
        REQUIRE_CALL(stdUtilsMock, FileReadAllText(remove_stamina_mod_cpp_path)).LR_RETURN(
            remove_stamina_mod_cpp_no_whitespaces);

        WHEN("Mod is created")
        {
            Mod remove_stamina(remove_stamina_path);
            THEN("Appropriate keys and values are set, despite no whitespaces")
            {
                CHECK_EQ(remove_stamina_map, remove_stamina.KeyValue);
            }
        }
    }
}

TEST_CASE_FIXTURE(ModTests, "Bunch of nasty parse strings")
{
    std::string const mod_cpp = R"cpp(name = "Remove\"Stamina";
  this_is_indented_with_few_spaces   = "Hey there";
evenNewLineCapable=It can also load without quotes;)cpp";
    std::map<std::string, std::string> const mod_map
    {
        {"evenNewLineCapable", "Itcanalsoloadwithoutquotes"},
        {"name", "\"Remove\\\"Stamina"},
        {"publishedid", "remove_stamina"},
        {"this_is_indented_with_few_spaces", "Hey there"}
    };

    REQUIRE_CALL(filesystemUtilsMock, Ls(remove_stamina_path, _)).TIMES(2).RETURN(std::vector<std::string> {"addons", "mod.cpp"});
    REQUIRE_CALL(stdUtilsMock, FileReadAllText(remove_stamina_mod_cpp_path)).LR_RETURN(mod_cpp);

    Mod mod(remove_stamina_path);
    CHECK_EQ(mod_map, mod.KeyValue);
}
