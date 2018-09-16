#include "gtest/gtest.h"

#include "mod.hpp"
#include "setup.hpp"

class ModTests : public ::testing::Test
{
    public:
        std::string dir = GetWorkDir() + "/test-files";
        std::string arma3_dir = "/arma3/!workshop";
        std::string remove_stamina_dir = "/@Remove stamina";
        std::string big_mod_dir = "/@bigmod";
};

TEST_F(ModTests, BasicParser)
{
    Mod remove_stamina{.path_ = dir + arma3_dir + remove_stamina_dir, {}};
    remove_stamina.LoadFromFile(dir + arma3_dir + remove_stamina_dir + "/mod.cpp");
    ASSERT_EQ(remove_stamina_map, remove_stamina.KeyValue);
    ASSERT_EQ(dir + arma3_dir + remove_stamina_dir, remove_stamina.path_);

    Mod big_mod{.path_ = dir + arma3_dir + big_mod_dir, {}};
    big_mod.LoadFromFile(dir + arma3_dir + big_mod_dir + "/mod.cpp");
    ASSERT_EQ(big_mod_map, big_mod.KeyValue);
    ASSERT_EQ(dir + arma3_dir + big_mod_dir, big_mod.path_);
}

TEST_F(ModTests, MissingQuotesAndWhitespaces)
{
    Mod remove_stamina_missing_quotes{.path_ = dir, {}};
    remove_stamina_missing_quotes.LoadFromFile(dir + "/mod-remove-stamina-missing-quotes.cpp");
    Mod remove_stamina_no_whitespaces{.path_ = dir, {}};
    remove_stamina_no_whitespaces.LoadFromFile(dir + "/mod-remove-stamina-no-whitespaces.cpp");

    ASSERT_EQ(remove_stamina_map, remove_stamina_missing_quotes.KeyValue);
    ASSERT_EQ(remove_stamina_map, remove_stamina_no_whitespaces.KeyValue);
}
