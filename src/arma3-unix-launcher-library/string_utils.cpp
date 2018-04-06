#include "string_utils.hpp"

namespace StringUtils
{
    size_t find_last_nth(const std::string &text, const char c, int count = 1)
    {
        ssize_t found_chars = 0;
        ssize_t found_i = 0;
        for (ssize_t i = text.size() - 1; i >= 0; i--)
        {
            if (text[i] == c)
            {
                found_chars++;
                found_i = i;
            }
            if (found_chars == count)
                return found_i;
        }
        if (found_chars > 0)
            return found_i;
        return std::string::npos;
    }

    std::string_view RemoveElementsFromPath(const std::string &text, bool remove_slash, int count)
    {
        if (text.length() == 0)
            return std::string_view(text.c_str(), text.size());

        size_t pos = find_last_nth(text, '/', count);
        if (pos == std::string::npos)
            return std::string_view(text.c_str(), text.size());
        if (!remove_slash)
            pos++;
        return std::string_view(text.c_str(), pos);
    }

    std::string Replace(const std::string &text, const std::string &from, const std::string &to)
    {
        return "";
    }

    bool EndsWith(const std::string &text, const std::string &find)
    {
        if (find.size() > text.size())
            return false;
        return find == std::string_view(text.c_str() + text.size() - find.size(), find.size());
    }

    bool StartsWith(const std::string &text, const std::string &find)
    {
        if (find.size() > text.size())
            return false;
        return find == std::string_view(text.c_str(), find.size());
    }

    std::vector<std::string_view> Split(const std::string &text_to_split, const char delimiter)
    {
        return std::vector<std::string_view>();
    }
}
