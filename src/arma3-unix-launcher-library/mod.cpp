#include "mod.hpp"

#include <iostream>
#include <regex>
#include <string_view>

#include "filesystem.hpp"
#include "string_utils.hpp"

void Mod::LoadAllCPP()
{
    KeyValue.clear();
    for (const auto &cppfile : Filesystem::Ls(path_))
    {
        if (StringUtils::EndsWith(cppfile, ".cpp"))
            LoadFromFile(path_ + "/" + cppfile, true);
    }
}

void Mod::LoadFromFile(const std::string &path, bool append)
{
    return LoadFromText(Filesystem::FileReadAllText(path), append);
}

void Mod::LoadFromText(const std::string &text, bool append)
{
    if (!append)
        KeyValue.clear();
    ParseCPP(RemoveWhitespacesAndComments(text));
}

std::string Mod::RemoveWhitespacesAndComments(const std::string &text)
{
    std::regex remove_whitespaces("\\s+(?=([^\"]*\"[^\"]*\")*[^\"]*$)");
    std::regex remove_comments("\\s+(?=([^\"]*\"[^\"]*\")*[^\"]*$)");
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
