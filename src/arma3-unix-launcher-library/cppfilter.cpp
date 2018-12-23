#include "cppfilter.hpp"

#include <fmt/format.h>
#include <string_view>

#include "exceptions/syntax_error.hpp"

std::string CppFilter::RemoveClass(const std::string &class_name)
{
    auto occurences = FindAllClassOccurences(class_name);
    if (occurences.size() == 0)
        return class_text_;

    std::string ret = class_text_;

    for (int i = static_cast<int>(occurences.size()) - 1; i >= 0; --i)
    {
        auto boundaries = GetClassBoundaries(class_name, occurences[i]);
        fmt::print("b s: {} e: {}\n", boundaries.first, boundaries.second);
        ret = ret.substr(0, boundaries.first) + ret.substr(boundaries.second);
    }

    return ret;
}

std::vector<size_t> CppFilter::FindAllClassOccurences(const std::string &class_name)
{
    std::vector<size_t> ret;
    size_t pos = class_text_.find(class_name);
    while (pos != std::string::npos)
    {
        ret.push_back(pos);
        pos = class_text_.find(class_name, pos + class_name.size());
    }
    return ret;
}

std::pair<size_t, size_t> CppFilter::GetClassBoundaries(std::string const &class_name, size_t start)
{
    std::string_view view(class_text_.c_str() + start, class_name.size());
    fmt::print("class_name: {}\nview: {}\n", class_name, view);
    if (view != class_name)
        throw SyntaxErrorException("Cannot find class name");

    int bracket_depth = 1;
    size_t bracket_pos = class_text_.find('{', start);
    if (bracket_pos == std::string::npos)
        throw SyntaxErrorException("Cannot find opening bracket");

    size_t pos = bracket_pos + 1;

    bool escape = false;
    bool in_string = false;
    while (pos < class_text_.size() && bracket_depth > 0)
    {
        char c = class_text_[pos];

        if (escape)
            escape = false;
        else if (in_string && c == '\\')
            escape = true;
        else if (c == '"')
            in_string = !in_string;
        else if (!in_string)
        {
            if (c == '{')
                ++bracket_depth;
            else if (c == '}')
                --bracket_depth;
        }
        ++pos;
    }

    if (bracket_depth != 0)
        throw SyntaxErrorException("Unclosed bracket");

    return std::make_pair(start, pos + 2);
}

#ifndef DOCTEST_CONFIG_DISABLE
//GCOV_EXCL_START
#include "doctest.h"
#include "tests.hpp"

TEST_SUITE_BEGIN("CppFilter");

TEST_CASE("RemoveModLauncherList")
{
    std::array<std::string, 8> config_files =
    {

        "",
        "some trash",
        Tests::Utils::valid_config_file,
        Tests::Utils::valid_config_file + Tests::Utils::mod_classes,
        Tests::Utils::valid_config_file + Tests::Utils::unrelated_classes,
        Tests::Utils::valid_config_file + Tests::Utils::mod_classes + Tests::Utils::unrelated_classes,
        Tests::Utils::valid_config_file + Tests::Utils::error_prone_classes,
        Tests::Utils::valid_config_file + Tests::Utils::error_prone_classes + Tests::Utils::mod_classes
    };

    std::array<std::string, 8> out_files =
    {
        "",
        "some trash",
        Tests::Utils::valid_config_file,
        Tests::Utils::valid_config_file,
        Tests::Utils::valid_config_file + Tests::Utils::unrelated_classes,
        Tests::Utils::valid_config_file + Tests::Utils::unrelated_classes,
        Tests::Utils::valid_config_file + Tests::Utils::error_prone_classes,
        Tests::Utils::valid_config_file + Tests::Utils::error_prone_classes
    };

    for (size_t i = 0; i < config_files.size(); ++i)
    {
        CppFilter cpp_filter{config_files[i]};
        CHECK_EQ(out_files[i], cpp_filter.RemoveClass("class ModLauncherList"));
    }
}

TEST_SUITE_END();

//GCOV_EXCL_STOP
#endif
