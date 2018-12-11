#include "vdf.hpp"

#include <filesystem>
#include <string>

#include "exceptions/syntax_error.hpp"

#include "std_utils.hpp"

VDF::VDF() noexcept
{
}

std::vector<std::string> VDF::GetValuesWithFilter(std::string filter)
{
    std::vector<std::string> result;
    for (const auto &[key, value] : KeyValue)
    {
        if (key.find(filter) != std::string::npos)
            result.emplace_back(value);
    }
    return result;
}

void VDF::LoadFromFile(const std::filesystem::path &path, bool append)
{
    std::string text = StdUtils::FileReadAllText(path);
    LoadFromText(text, append);
}

void VDF::LoadFromText(const std::string &text, bool append)
{
    if (!append)
        KeyValue.clear();
    ParseVDF(RemoveWhitespaces(text));
}

void VDF::AddKeyValuePair()
{
    std::string key_path = "";
    for (const auto &str : hierarchy_)
        key_path += str + "/";
    key_path += key_;
    KeyValue[key_path] = value_;
    key_ = "";
    value_ = "";
}

std::string VDF::RemoveWhitespaces(const std::string &text)
{
    /*
     * This crashes, is the string too long?
     * std::regex regex("\\s+(?=([^\"]*\"[^\"]*\")*[^\"]*$)");
     * std::string text_without_whitespaces = std::regex_replace(text, regex, "");
     * return text_without_whitespaces;
     *
     * Anyways, here is very dumb implementation
     */
    std::string ret = "";
    ret.reserve(text.length());
    bool in_quotes = false;
    for (auto c : text)
    {
        if (c == '"')
            in_quotes = !in_quotes;
        if (!std::isspace(c) || in_quotes)
            ret.push_back(c);
    }
    return ret;
}

void VDF::ParseVDF(const std::string &text)
{
    state_ = VDFState::LookingForKey;
    key_ = "";
    value_ = "";
    hierarchy_.clear();
    for (auto c : text)
        ProcessChar(c);

    if (hierarchy_.size() != 0)
        throw SyntaxErrorException("Unclosed brackets in VDF");
}

void VDF::ProcessChar(char c)
{
    if (state_ == VDFState::LookingForKey)
        LookForKey(c);
    else if (state_ == VDFState::LookingForValue)
        LookForValue(c);
    else if (state_ == VDFState::ReadingKey)
        ReadKey(c);
    else if (state_ == VDFState::ReadingValue)
        ReadValue(c);
}

void VDF::LookForKey(char c)
{
    if (c == '"')
        state_ = VDFState::ReadingKey;
    else if (c == '}')
        hierarchy_.pop_back();
    else
        throw SyntaxErrorException("VDF: Quote or bracket expected");
}

void VDF::LookForValue(char c)
{
    if (c == '"')
        state_ = VDFState::ReadingValue;
    else if (c == '{')
    {
        hierarchy_.emplace_back(key_);
        key_ = "";
        state_ = VDFState::LookingForKey;
    }
    else if (c == '}')
    {
        hierarchy_.pop_back();
        key_ = "";
        state_ = VDFState::LookingForKey;
    }
    else
        throw SyntaxErrorException("VDF: Quote or bracket expected");
}

void VDF::ReadKey(char c)
{
    if (c != '"')
        key_ += c;
    else
        state_ = VDFState::LookingForValue;
}

void VDF::ReadValue(char c)
{
    if (c != '"')
        value_ += c;
    else
    {
        state_ = VDFState::LookingForKey;
        AddKeyValuePair();
    }
}

#ifndef DOCTEST_CONFIG_DISABLE
//GCOV_EXCL_START
#include <doctest.h>
#include "tests.hpp"

TEST_SUITE_BEGIN("VDF");

TEST_CASE_FIXTURE(Tests::Fixture, "BasicFilter")
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

TEST_CASE_FIXTURE(Tests::Fixture, "BasicParser")
{
    GIVEN("Empty VDF")
    {
        VDF vdf;
        WHEN("\"Key\" \"Value\"")
        {
            std::string simple_key_value = "\"Key\"\"Value\"";
            vdf.LoadFromText(simple_key_value);
            THEN("Key points to Value")
            {
                CHECK_EQ("Value", vdf.KeyValue["Key"]);
                CHECK_EQ(static_cast<size_t>(1), vdf.KeyValue.size());
            }
        }

        WHEN("\"Branch\" { \"Key\" \"Value\" }")
        {
            std::string simple_key_value = "\"Branch\"{\"Key\"\"Value\"}";
            vdf.LoadFromText(simple_key_value);
            THEN("Branch\\Key points to Value")
            {
                CHECK_EQ("Value", vdf.KeyValue["Branch/Key"]);
                CHECK_EQ(static_cast<size_t>(1), vdf.KeyValue.size());
            }
        }
    }
}

TEST_CASE_FIXTURE(Tests::Fixture, "LoadFromFile")
{
    GIVEN("Two empty VDFs")
    {
        VDF vdf, vdfWithTabs;
        WHEN("load VDF 1 with valid file, load VDF 2 with file using mixed spaces and tabs")
        {
            vdf.LoadFromFile(test_files_path / "vdf-valid.vdf");
            vdfWithTabs.LoadFromFile(test_files_path / "vdf-valid-mixed-spaces-with-tabs.vdf");
            THEN("Both VDFs should be equal")
            {
                CHECK_EQ(vdf.KeyValue, vdfWithTabs.KeyValue);
                CHECK_EQ(static_cast<size_t>(8), vdf.KeyValue.size());
            }
        }
    }
}

TEST_CASE_FIXTURE(Tests::Fixture, "ParserThenFilter")
{
    GIVEN("Valid Steam VDF with various key-value pairs, list of valid paths")
    {
        VDF vdf;
        vdf.LoadFromFile(test_files_path / "vdf-valid.vdf");
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

TEST_SUITE_END();

//GCOV_EXCL_STOP
#endif
