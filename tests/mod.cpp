#include "gtest/gtest.h"

#include "filesystem.hpp"
#include "mod.hpp"
#include "setup.hpp"

class ModTests : public ::testing::Test
{
public:
    std::string dir = GetWorkDir() + "/test-files";
};

TEST_F(ModTests, BasicParser)
{
    std::map<std::string, std::string> remove_stamina_map
    {
        {"name", "Remove Stamina"},
        {"picture", "logo.paa"},
        {"hidePicture", "false"},
        {"hideName", "false"},
        {"logo", "logo.paa"},
        {"description", "Simple mod which removes stamina from ArmA 3"},
        {"author", "Muttley"}
    };

    std::map<std::string, std::string> big_mod_map
    {
        {"dir", "@bigmod"},
        {"name", "Big Mod"},
        {"picture", "bigmod\\addons\\not\\nice\\path\\with\\backslashes\\picture.paa"},
        {"actionName", "$STR_MOD_LAUNCHER_ACTION"},
        {"action", "https://bigmod.somewhere"},
        {"description", "Bugtracker: https://bigmod.somewhere/issues<br/>Documentation: https://bigmod.somewhere/wiki"},
        {"hideName", "1"},
        {"hidePicture", "0"},
        {"logo", "bigmod\\addons\\not\\nice\\path\\with\\backslashes\\logo.paa"},
        {"logoOver", "bigmod\\addons\\not\\nice\\path\\with\\backslashes\\logo.paa"},
        {"tooltip", "Big Mod"},
        {"tooltipOwned", "Big Mod Owned"},
        {"overview", "Big Mod is very Big."},
        {"author", "Big Modders"},
        {"overviewPicture", "bigmod\\addons\\not\\nice\\path\\with\\backslashes\\logo.paa"},
        {"overviewText", "Big Mod for Arma 3"},
        {"overviewFootnote", "<br /><br /><t color='#aa00aa'>Some random bla bla with HTML tags.<br />This will be <t /><t color='#ffaa00'>annoying<t /> to parse"},
        {"version", "1.0a"}
    };

    Mod remove_stamina_mod(dir + "/mod-remove-stamina.cpp");
    Mod big_mod_mod(dir + "/mod-all-keys.cpp");
    ASSERT_EQ(remove_stamina_mod.KeyValue, remove_stamina_map);
    ASSERT_EQ(big_mod_mod.KeyValue, big_mod_map);
}

TEST_F(ModTests, MissingQuotesAndWhitespaces)
{
    std::map<std::string, std::string> remove_stamina_map
    {
        {"name", "Remove Stamina"},
        {"picture", "logo.paa"},
        {"hidePicture", "false"},
        {"hideName", "false"},
        {"logo", "logo.paa"},
        {"description", "Simple mod which removes stamina from ArmA 3"},
        {"author", "Muttley"}
    };
    Mod remove_stamina_missing_quotes_mod(dir + "/mod-remove-stamina-missing-quotes.cpp");
    Mod remove_stamina_no_whitespaces_mod(dir + "/mod-remove-stamina-no-whitespaces.cpp");

    ASSERT_EQ(remove_stamina_missing_quotes_mod.KeyValue, remove_stamina_map);
    ASSERT_EQ(remove_stamina_no_whitespaces_mod.KeyValue, remove_stamina_map);
}

TEST_F(ModTests, MissingSemicolon)
{
    Mod remove_stamina_missing_semicolon;
    errno = 0;
    int result = remove_stamina_missing_semicolon.LoadFromFile(dir + "/mod-remove-stamina-missing-semicolon.cpp");
    ASSERT_EQ(result, -1);
    EXPECT_EQ(errno, EFAULT);
}
