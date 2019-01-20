#ifndef STRING_UTILS_HPP_
#define STRING_UTILS_HPP_

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

    std::filesystem::path ToWindowsPath(std::filesystem::path const &path);

    template<typename ret_type = std::string_view, typename argument = std::string>
    ret_type TrimLeft(argument const &text, std::string const &to_trim = " \n\r\t")
    {
        size_t startpos = text.find_first_not_of(to_trim);
        if constexpr(std::is_same_v<ret_type, std::string_view> &&std::is_same_v<argument, std::string>)
            return (startpos == std::string::npos) ? std::string_view() : std::string_view(text.c_str() + startpos,
                    text.size() - startpos);
        else //std::string ret_type
            return (startpos == std::string::npos) ? "" : text.substr(startpos);
    }

    template<typename ret_type = std::string_view, typename argument = std::string>
    ret_type TrimRight(argument const &text, std::string const &to_trim = " \n\r\t")
    {
        size_t endpos = text.find_last_not_of(to_trim);
        if constexpr(std::is_same_v<ret_type, std::string_view> &&std::is_same_v<argument, std::string>)
            return (endpos == std::string::npos) ? std::string_view() : std::string_view(text.c_str(), endpos + 1);
        else //std::string ret_type
            return (endpos == std::string::npos) ? "" : text.substr(0, endpos + 1);
    }

    template<typename ret_type = std::string_view, typename argument = std::string>
    ret_type Trim(argument const &text, std::string const &to_trim = " \n\r\t")
    {
        return TrimLeft<ret_type, ret_type>(TrimRight<ret_type, argument>(text, to_trim));
    }
}

#endif
