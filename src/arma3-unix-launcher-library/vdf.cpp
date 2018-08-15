#include "vdf.hpp"

#include <string>
#include <regex>

#include "filesystem.hpp"

#include "exceptions/syntax_error.hpp"

VDF::VDF() noexcept
{
}

std::vector<std::string> VDF::GetValuesWithFilter(std::string filter)
{
    std::vector<std::string> result;
    for (const auto &[key, value] : KeyValue)
    {
        if (key.find(filter) != std::string::npos)
            result.emplace_back(value);
    }
    return result;
}

void VDF::LoadFromFile(const std::string &path, bool append)
{
    std::string text = Filesystem::FileReadAllText(path);
    LoadFromText(text, append);
}

void VDF::LoadFromText(const std::string &text, bool append)
{
    if (!append)
        KeyValue.clear();
    ParseVDF(RemoveWhitespaces(text));
}

void VDF::AddKeyValuePair(std::vector<std::string> &hierarchy, std::string &key, std::string &value)
{
    std::string key_path = "";
    for (const auto &str : hierarchy)
        key_path += str + "/";
    key_path += key;
    KeyValue[key_path] = value;
    key = "";
    value = "";
}

std::string VDF::RemoveWhitespaces(const std::string &text)
{
    /*
     * This crashes, is the string too long?
     * std::regex regex("\\s+(?=([^\"]*\"[^\"]*\")*[^\"]*$)");
     * std::string text_without_whitespaces = std::regex_replace(text, regex, "");
     * return text_without_whitespaces;
     *
     * Anyways, here is very dumb implementation
     */
    std::string ret = "";
    ret.reserve(text.length());
    bool in_quotes = false;
    for (auto c : text)
    {
        if (c == '"')
            in_quotes = !in_quotes;
        if (!std::isspace(c) || in_quotes)
            ret.push_back(c);
    }
    return ret;
}

void VDF::ParseVDF(const std::string &text)
{
    state_ = VDFState::LookingForKey;
    key = "";
    value = "";
    hierarchy.clear();
    for (auto c : text)
        ProcessChar(c);

    if (hierarchy.size() != 0)
        throw SyntaxErrorException("Unclosed brackets in VDF");
}

void VDF::ProcessChar(char c)
{
    if (state_ == VDFState::LookingForKey)
        LookForKey(c);
    else if (state_ == VDFState::LookingForValue)
        LookForValue(c);
    else if (state_ == VDFState::ReadingKey)
        ReadKey(c);
    else if (state_ == VDFState::ReadingValue)
        ReadValue(c);
}

void VDF::LookForKey(char c)
{
    if (c == '"')
        state_ = VDFState::ReadingKey;
    else if (c == '}')
        hierarchy.pop_back();
    else
        throw SyntaxErrorException("VDF: Quote or bracket expected");
}

void VDF::LookForValue(char c)
{
    if (c == '"')
        state_ = VDFState::ReadingValue;
    else if (c == '{')
    {
        hierarchy.emplace_back(key);
        key = "";
        state_ = VDFState::LookingForKey;
    }
    else if (c == '}')
    {
        hierarchy.pop_back();
        key = "";
        state_ = VDFState::LookingForKey;
    }
    else
        throw SyntaxErrorException("VDF: Quote or bracket expected");
}

void VDF::ReadKey(char c)
{
    if (c != '"')
        key += c;
    else
        state_ = VDFState::LookingForValue;
}

void VDF::ReadValue(char c)
{
    if (c != '"')
        value += c;
    else
    {
        state_ = VDFState::LookingForKey;
        AddKeyValuePair(hierarchy, key, value);
    }
}
