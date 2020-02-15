/*
 * Every project has this dark place, where code gets dirty
 * this is the place, tread carefeully
 */

#include "cppfilter.hpp"

#include <fmt/format.h>
#include <string_view>

#include "exceptions/syntax_error.hpp"

std::string CppFilter::RemoveClass(std::string const &class_name)
{
    auto occurences = FindAllClassOccurences(class_name);
    if (occurences.size() == 0)
        return class_text_;

    std::string ret = class_text_;

    for (int i = static_cast<int>(occurences.size()) - 1; i >= 0; --i)
    {
        auto boundaries = GetClassBoundaries(class_name, occurences[i]);
        ret = ret.substr(0, boundaries.first) + ret.substr(boundaries.second);
    }

    return ret;
}

std::vector<size_t> CppFilter::FindAllClassOccurences(std::string const &class_name)
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

    return std::make_pair(start, GetColonNewlineOrChar(pos));
}

size_t CppFilter::GetColonNewlineOrChar(size_t pos)
{
    bool colon_found = false;
    bool newline_found = false;
    bool char_found = false;

    while (pos < class_text_.size())
    {
        char c = class_text_[pos];
        if (c == ';')
            colon_found = true;
        else if (c == '\n')
            newline_found = true;
        else if (isalnum(c))
            char_found = true;

        if (colon_found && newline_found)
            return pos + 1;
        else if (colon_found && char_found)
            return pos;

        ++pos;
    }

    return std::string::npos;
}
