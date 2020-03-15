#pragma once

#include <filesystem>
#include <type_traits>
#include <string>
#include <string_view>
#include <vector>

namespace StringUtils
{
    std::string_view RemoveElementsFromPath(std::string const &text, bool remove_slash = true, int count = 1);
    std::string Replace(std::string text, std::string const &from, std::string const &to);

    bool EndsWith(std::string const &text, std::string const &find);
    bool StartsWith(std::string const &text, std::string const &find);

    std::vector<std::string_view> Split(std::string const &text_to_split, std::string const &delimiters);

    std::string Lowercase(std::string text);

    std::filesystem::path ToWindowsPath(std::filesystem::path const &path, char const drive_letter = 'C');

    constexpr std::string_view trim_left(std::string_view text,
                                         std::string_view const to_trim = std::string_view(" \n\r\t\0", 5))
    {
        text.remove_prefix(std::min(text.find_first_not_of(to_trim), text.size()));
        return text;
    }

    constexpr std::string_view trim_right(std::string_view text,
                                          std::string_view const to_trim = std::string_view(" \n\r\t\0", 5))
    {
        size_t const end_pos = text.find_last_not_of(to_trim);
        if (end_pos != std::string_view::npos)
            text.remove_suffix(text.length() - end_pos - 1);

        return text;
    }

    constexpr std::string_view trim(std::string_view text,
                                    std::string_view const to_trim = std::string_view(" \n\r\t\0", 5))
    {
        return trim_left(trim_right(text, to_trim));
    }
}
