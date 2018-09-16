#include "string_utils.hpp"

#include <algorithm>
#include <iostream>

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

    std::string Replace(std::string text, const std::string &from, const std::string &to)
    {
        if (from.empty())
            return text;

        size_t start_pos = 0;
        while (start_pos != std::string::npos)
        {
            start_pos = text.find(from, start_pos);
            if (start_pos == std::string::npos)
                return text;
            text.replace(start_pos, from.length(), to);
            //move start_pos forward so it doesn't replace the same string over and over
            start_pos += to.length() - from.length() + 2;
        }
        return text;
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

    std::vector<std::string_view> Split(const std::string &text_to_split, const std::string &delimiters)
    {
        std::vector<std::string_view> ret;

        size_t start_trim_size = text_to_split.find_first_not_of(delimiters);
        size_t end_trim_size = text_to_split.find_last_not_of(delimiters);
        if (start_trim_size == std::string::npos || end_trim_size == std::string::npos)
            return ret;
        std::string_view trimmed(text_to_split.c_str() + start_trim_size, end_trim_size - start_trim_size + 1);

        size_t start_pos = 0;
        size_t end_pos = std::string::npos;
        while (true)
        {
            start_pos = trimmed.find_first_not_of(delimiters, start_pos);
            end_pos = trimmed.find_first_of(delimiters, start_pos);
            if (end_pos == std::string::npos)
            {
                ret.push_back(trimmed.substr(start_pos, trimmed.size() - start_pos));
                break;
            }
            ret.push_back(trimmed.substr(start_pos, end_pos - start_pos));
            start_pos = end_pos;
        }

        return ret;
    }

    std::string Lowercase(std::string text)
    {
        std::transform(text.begin(), text.end(), text.begin(), ::tolower);
        return text;
    }
}
