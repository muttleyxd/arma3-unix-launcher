#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "default_test_reporter.hpp"

#include "vdf.hpp"

constexpr std::string_view vdf_valid = R"vdf(
"VDFTests"
{
	"Branch1"
	{
		"SubBranch1"
		{
			"Application1"
			{
				"SomeKey"		"5"
				"SomeKeyWithValues"
				{
					"KeyValue"		"text"
					"KeyValueInt"		"155"
				}
			}
		}
		"SubBranch2"
		{
			"KeyValue" "8"
		}
	}
	"Branch2"
	{
		"SubBranch2"
		{
			"Directories"
			{
				"BaseInstallFolder_1"		"/mnt/games/SteamLibrary"
				"BaseInstallFolder_2"		"/home/user/SteamLibrary"
				"BaseInstallFolder_3"		"/run/media/user/SteamLibrary"
				"BaseInstallFolder_4"		"/somerandompath/steamlibrary"
			}
		}
	}
}
)vdf";

constexpr std::string_view vdf_valid_mixed_spaces_with_tabs = R"vdf(
"VDFTests"
{
  "Branch1"
	{
		"SubBranch1"
		{
		 "Application1"
  		  	{
				"SomeKey" "5"
				"SomeKeyWithValues"
				{
					"KeyValue"	"text"
					"KeyValueInt"        "155"
				}
			}
		}
		"SubBranch2"
{
  "KeyValue" "8"
}
	}
"Branch2"
{
 "SubBranch2"
    {
	   "Directories"
	    	{
		"BaseInstallFolder_1"		"/mnt/games/SteamLibrary"
			"BaseInstallFolder_2"		"/home/user/SteamLibrary"
				  	"BaseInstallFolder_3"		"/run/media/user/SteamLibrary"
	           			"BaseInstallFolder_4"		"/somerandompath/steamlibrary"
			}
		}
	}
}
)vdf";

constexpr std::string_view vdf_invalid_missing_brackets = R"vdf(
"VDFTests"
{
	"Branch1"
	{
		"SubBranch1"
			"Application1"
				"SomeKey"		"5"
				"SomeKeyWithValues"
				{
					"KeyValue"		"text"
					"KeyValueInt"		"155"
				}
			}
		}
		"SubBranch2"
			"KeyValue" "8"
	"Branch2"
	{
		"SubBranch2"
		{
			"Directories"
			{
				"BaseInstallFolder_1"		"/mnt/games/SteamLibrary"
				"BaseInstallFolder_2"		"/home/user/SteamLibrary"
				"BaseInstallFolder_3"		"/run/media/user/SteamLibrary"
				"BaseInstallFolder_4"		"/somerandompath/steamlibrary"
		}
)vdf";

TEST_CASE("BasicFilter")
{
    GIVEN("VDF filled with KeyX/ValueX pairs")
    {
        VDF vdf;
        std::string key = "Key";
        std::string value = "Value";
        for (int i = 0; i < 10; i++)
        {
            std::string number = std::to_string(i + 1);
            vdf.KeyValue[key + number] = value + number;
        }
        WHEN("Filtering VDF with non-existing filter")
        {
            THEN("Result should be empty (zero size)")
            {
                CHECK_EQ(static_cast<size_t>(0), vdf.GetValuesWithFilter("This should be empty").size());
            }
        }

        WHEN("Filtering VDF with filter matching all keys")
        {
            THEN("Result shold contain all values")
            {
                CHECK_EQ(vdf.KeyValue.size(), vdf.GetValuesWithFilter("Key").size());
            }
        }

        WHEN("Filtering VDF with filter matching two keys")
        {
            THEN("Result should contain two keys")
            {
                CHECK_EQ(static_cast<size_t>(2), vdf.GetValuesWithFilter("Key1").size());
            }
        }
    }
}

TEST_CASE("BasicParser")
{
    GIVEN("Empty VDF")
    {
        VDF vdf;
        WHEN("\"Key\" \"Value\"")
        {
            std::string simple_key_value = R"vdf("Key""Value")vdf";
            vdf.LoadFromText(simple_key_value);
            THEN("Key points to Value")
            {
                CHECK_EQ("Value", vdf.KeyValue["Key"]);
                CHECK_EQ(static_cast<size_t>(1), vdf.KeyValue.size());
            }
        }

        WHEN("\"Branch\" { \"Key\" \"Value\" }")
        {
            std::string simple_key_value = R"vdf("Branch"{"Key""Value"})vdf";
            vdf.LoadFromText(simple_key_value);
            THEN("Branch\\Key points to Value")
            {
                CHECK_EQ("Value", vdf.KeyValue["Branch/Key"]);
                CHECK_EQ(static_cast<size_t>(1), vdf.KeyValue.size());
            }
        }
    }
}

TEST_CASE("LoadFromFile")
{
    GIVEN("Two empty VDFs")
    {
        VDF vdf, vdfWithTabs;
        WHEN("load VDF 1 with valid file, load VDF 2 with file using mixed spaces and tabs")
        {
            vdf.LoadFromText(vdf_valid);
            vdfWithTabs.LoadFromText(vdf_valid_mixed_spaces_with_tabs);
            THEN("Both VDFs should be equal")
            {
                CHECK_EQ(vdf.KeyValue, vdfWithTabs.KeyValue);
                CHECK_EQ(static_cast<size_t>(8), vdf.KeyValue.size());
            }
        }
    }
}

TEST_CASE("ParserThenFilter")
{
    GIVEN("Valid Steam VDF with various key-value pairs, list of valid paths")
    {
        VDF vdf;
        vdf.LoadFromText(vdf_valid);
        std::vector<std::string> paths{"/mnt/games/SteamLibrary", "/home/user/SteamLibrary", "/run/media/user/SteamLibrary", "/somerandompath/steamlibrary"};

        WHEN("Filtering valid VDF by BaseInstallFolder and sorting output")
        {
            std::vector<std::string> filtered = vdf.GetValuesWithFilter("BaseInstallFolder");
            std::sort(filtered.begin(), filtered.end());
            std::sort(paths.begin(), paths.end());
            THEN("Sorted output should be equal to paths")
            {
                CHECK_EQ(filtered, paths);
            }
        }
    }
}
