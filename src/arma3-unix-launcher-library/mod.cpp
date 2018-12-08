#include "mod.hpp"

#include <filesystem>
#include <iostream>
#include <regex>
#include <string_view>

#include "std_utils.hpp"
#include "string_utils.hpp"

void Mod::LoadAllCPP()
{
    KeyValue.clear();
    for (const auto &cppfile : StdUtils::Ls(path_))
    {
        if (StringUtils::EndsWith(cppfile, ".cpp"))
            LoadFromFile(path_ + "/" + cppfile, true);
    }
}

void Mod::LoadFromFile(const std::string &path, bool append)
{
    return LoadFromText(StdUtils::FileReadAllText(path), append);
}

void Mod::LoadFromText(const std::string &text, bool append)
{
    if (!append)
        KeyValue.clear();
    ParseCPP(RemoveWhitespacesAndComments(text));
}

std::string Mod::RemoveWhitespacesAndComments(const std::string &text)
{
    std::regex remove_whitespaces("\\s+(?=([^\"]*\"[^\"]*\")*[^\"]*$)");
    std::regex remove_comments("\\s+(?=([^\"]*\"[^\"]*\")*[^\"]*$)");
    return std::regex_replace(std::regex_replace(text, remove_comments, ""), remove_whitespaces, "");
}

void Mod::ParseCPP(const std::string &text)
{
    std::vector<std::string_view> lines = StringUtils::Split(text, ";");
    for (const auto &line : StringUtils::Split(text, ";"))
    {
        size_t split_place = line.find('=');
        if (split_place == std::string_view::npos)
            continue;

        size_t value_start = split_place + 1;
        size_t value_end = line.length();
        if (line[value_start] == '"') // Remove quotes
            value_start++;
        if (line[value_end - 1] == '"')
            value_end--;

        KeyValue[std::string(line.substr(0, split_place))] = std::string(line.substr(value_start, value_end - value_start));
    }
}

#ifndef DOCTEST_CONFIG_DISABLE
//GCOV_EXCL_START
#include <doctest.h>
#include "tests.hpp"

class ModTests
{
    public:
        std::string dir = Tests::Utils::GetWorkDir() + "/test-files";
        std::string arma3_dir = "/arma3/!workshop";
        std::string remove_stamina_dir = "/@Remove stamina";
        std::string big_mod_dir = "/@bigmod";
};

TEST_CASE_FIXTURE(ModTests, "BasicParser")
{
    Mod remove_stamina{dir + arma3_dir + remove_stamina_dir, {}};
    remove_stamina.LoadFromFile(dir + arma3_dir + remove_stamina_dir + "/mod.cpp");
    CHECK_EQ(Tests::Utils::remove_stamina_map, remove_stamina.KeyValue);
    CHECK_EQ(dir + arma3_dir + remove_stamina_dir, remove_stamina.path_);

    Mod big_mod{dir + arma3_dir + big_mod_dir, {}};
    big_mod.LoadFromFile(dir + arma3_dir + big_mod_dir + "/mod.cpp");
    CHECK_EQ(Tests::Utils::big_mod_map, big_mod.KeyValue);
    CHECK_EQ(dir + arma3_dir + big_mod_dir, big_mod.path_);
}

TEST_CASE_FIXTURE(ModTests, "MissingQuotesAndWhitespaces")
{
    Mod remove_stamina_missing_quotes{dir, {}};
    remove_stamina_missing_quotes.LoadFromFile(dir + "/mod-remove-stamina-missing-quotes.cpp");
    Mod remove_stamina_no_whitespaces{dir, {}};
    remove_stamina_no_whitespaces.LoadFromFile(dir + "/mod-remove-stamina-no-whitespaces.cpp");

    CHECK_EQ(Tests::Utils::remove_stamina_map, remove_stamina_missing_quotes.KeyValue);
    CHECK_EQ(Tests::Utils::remove_stamina_map, remove_stamina_no_whitespaces.KeyValue);
}

//GCOV_EXCL_STOP
#endif
