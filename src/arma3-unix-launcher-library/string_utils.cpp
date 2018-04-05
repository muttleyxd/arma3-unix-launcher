#include "string_utils.hpp"

namespace StringUtils
{
    size_type find_last_nth(const std::string &text, const char c, int count)
    {

    }

    std::string_view RemoveElementsFromPath(std::string text, bool remove_slash, int count)
    {
        if (text.length() == 0)
            return text;



        return text;
    }

    std::string Replace(const std::string &text, const std::string &from, const std::string &to)
    {
        return "";
    }

    std::string Trim(const std::string &text)
    {
        return "";
    }

    std::string TrimLeft(const std::string &text)
    {
        return "";
    }

    std::string TrimRight(const std::string &text)
    {
        return "";
    }

    bool EndsWith(const std::string &text, const std::string &find)
    {
        return false;
    }

    bool StartsWith(const std::string &text, const std::string &find)
    {
        return false;
    }

    std::vector<std::string_view> Split(const std::string &text_to_split, const char delimiter)
    {
        return std::vector<std::string_view>();
    }
}
