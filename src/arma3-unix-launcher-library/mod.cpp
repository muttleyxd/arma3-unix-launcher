#include "mod.hpp"

#include <filesystem>
#include <iostream>
#include <regex>
#include <string_view>

#include "exceptions/directory_not_found.hpp"

#include "filesystem_utils.hpp"
#include "std_utils.hpp"
#include "string_utils.hpp"

namespace fs = FilesystemUtils;

Mod::Mod(std::filesystem::path const &path) : path_(path)
{
    if (!StdUtils::Contains(fs::Ls(path, true), "addons"))
        throw DirectoryNotFoundException(path / "addons");

    LoadAllCPP();
    if (!StdUtils::ContainsKey(KeyValue, "publishedid") || KeyValue["publishedid"] == "0")
        KeyValue["publishedid"] = path.filename();
}

std::string Mod::GetName() const
{
    return GetValueOrReturnDefault("name", "dir", "tooltip", "publishedid", path_.filename());
}

void Mod::LoadAllCPP()
{
    for (auto const &cppfile : fs::Ls(path_))
        if (StringUtils::EndsWith(cppfile, ".cpp"))
            LoadFromText(StdUtils::FileReadAllText(path_ / cppfile), true);
}

void Mod::LoadFromText(std::string const &text, bool append)
{
    if (!append)
        KeyValue.clear();
    ParseCPP(RemoveWhitespacesAndComments(text));
}

bool Mod::IsWorkshopMod(std::filesystem::path const &workshop_path) const
{
    return path_.string().find(workshop_path) != std::string::npos;
}

std::string Mod::RemoveWhitespacesAndComments(std::string const &text)
{
    std::regex remove_whitespaces(R"(\s+(?=([^"]*"[^"]*")*[^"]*$))");
    std::regex remove_comments(R"(\s+(?=([^"]*"[^"]*")*[^"]*$))");
    return std::regex_replace(std::regex_replace(text, remove_comments, ""), remove_whitespaces, "");
}

void Mod::ParseCPP(std::string const &text)
{
    for (auto const &line : StringUtils::Split(text, ";"))
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

        std::string const key(StringUtils::trim(line.substr(0, split_place)));
        std::string const value(StringUtils::trim(line.substr(value_start, value_end - value_start)));
        KeyValue[key] = value;
    }
}
