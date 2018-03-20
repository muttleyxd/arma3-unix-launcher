#ifndef __STRING_UTILS_HPP
#define __STRING_UTILS_HPP

#include <string>
#include <string_view>
#include <vector>

namespace StringUtils
{
    std::string RemoveElementsFromPath(const std::string &text, bool remove_slash = true, int count = 1);
    std::string Replace(const std::string &text, const std::string &from, const std::string &to);
    std::string Trim(const std::string &text);
    std::string TrimLeft(const std::string &text);
    std::string TrimRight(const std::string &text);

    bool EndsWith(const std::string &text, const std::string &find);
    bool StartsWith(const std::string &text, const std::string &find);

    std::vector<std::string_view> Split(const std::string &text_to_split, const char delimiter);
}

#endif
