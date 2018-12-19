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

void Mod::LoadFromFile(const std::filesystem::path &path, bool append)
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
    std::regex remove_whitespaces(R"(\s+(?=([^"]*"[^"]*")*[^"]*$))");
    std::regex remove_comments(R"(\s+(?=([^"]*"[^"]*")*[^"]*$))");
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

std::ostream& operator<< (std::ostream& os, const std::map<std::string, std::string> key_value)
{
    os << "\nMap begin:\n";
    for (auto const& [key, value] : key_value)
        os << key << ':' << value << '\n';
    os << "Map end:\n\n";
    return os;
}

TEST_SUITE_BEGIN("Mod");

TEST_CASE_FIXTURE(Tests::Fixture, "BasicParser")
{
    std::filesystem::path workshop_path = test_files_path
            / arma3_dir
            / workshop_dir;

    std::filesystem::path remove_stamina_path = workshop_path / remove_stamina_dir;
    Mod remove_stamina{remove_stamina_path, {}};
    remove_stamina.LoadFromFile(remove_stamina_path / "mod.cpp");
    CHECK_EQ(Tests::Utils::remove_stamina_map, remove_stamina.KeyValue);
    CHECK_EQ(remove_stamina_path, remove_stamina.path_);

    std::filesystem::path big_mod_path = workshop_path / big_mod_dir;
    Mod big_mod{big_mod_path, {}};
    big_mod.LoadFromFile(big_mod_path / "mod.cpp");
    CHECK_EQ(Tests::Utils::big_mod_map, big_mod.KeyValue);
    CHECK_EQ(big_mod_path, big_mod.path_);
}

TEST_CASE_FIXTURE(Tests::Fixture, "MissingQuotesAndWhitespaces")
{
    Mod remove_stamina_missing_quotes{test_files_path, {}};
    remove_stamina_missing_quotes.LoadFromFile(test_files_path / "mod-remove-stamina-missing-quotes.cpp");
    Mod remove_stamina_no_whitespaces{test_files_path, {}};
    remove_stamina_no_whitespaces.LoadFromFile(test_files_path / "mod-remove-stamina-no-whitespaces.cpp");

    CHECK_EQ(Tests::Utils::remove_stamina_map, remove_stamina_missing_quotes.KeyValue);
    CHECK_EQ(Tests::Utils::remove_stamina_map, remove_stamina_no_whitespaces.KeyValue);
}

TEST_SUITE_END();

//GCOV_EXCL_STOP
#endif
