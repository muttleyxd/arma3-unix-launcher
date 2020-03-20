#include "vdf.hpp"

#include "exceptions/syntax_error.hpp"

#include "std_utils.hpp"

#include <fmt/format.h>

std::vector<std::string> VDF::GetValuesWithFilter(std::string_view const filter)
{
    std::vector<std::string> result;
    for (auto const &[key, value] : KeyValue)
    {
        if (key.find(filter) != std::string::npos)
            result.emplace_back(value);
    }
    return result;
}

void VDF::LoadFromText(std::string_view const text, bool append)
{
    if (!append)
        KeyValue.clear();
    ParseVDF(RemoveWhitespaces(text));
}

void VDF::AddKeyValuePair()
{
    std::string key_path;
    for (auto const &str : hierarchy_)
        key_path += str + "/";
    key_path += key_;
    KeyValue[key_path] = value_;
    key_ = "";
    value_ = "";
}

std::string VDF::RemoveWhitespaces(std::string_view const text)
{
    /*
     * This crashes, is the string too long?
     * std::regex regex("\\s+(?=([^\"]*\"[^\"]*\")*[^\"]*$)");
     * std::string text_without_whitespaces = std::regex_replace(text, regex, "");
     * return text_without_whitespaces;
     *
     * Anyways, here is very dumb implementation
     */
    std::string ret;
    ret.reserve(text.length());
    bool in_quotes = false;
    for (auto c : text)
    {
        if (c == '"')
            in_quotes = !in_quotes;
        if (!std::isspace(c) || in_quotes)
            ret += c;
    }
    return ret;
}

void VDF::ParseVDF(std::string const &text)
{
    state_ = VDFState::LookingForKey;
    key_ = "";
    value_ = "";
    hierarchy_.clear();
    for (auto c : text)
        ProcessChar(c);

    if (!hierarchy_.empty())
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
    else if (c == '}' && CanPop())
        hierarchy_.pop_back();
    else
        throw SyntaxErrorException("VDF: Quote or bracket expected");
}

void VDF::LookForValue(char c)
{
    if (c == '"')
        state_ = VDFState::ReadingValue;
    else if (c == '{')
    {
        hierarchy_.emplace_back(key_);
        key_ = "";
        state_ = VDFState::LookingForKey;
    }
    else if (c == '}' && CanPop())
    {
        hierarchy_.pop_back();
        key_ = "";
        state_ = VDFState::LookingForKey;
    }
    else
        throw SyntaxErrorException("VDF: Quote or bracket expected");
}

void VDF::ReadKey(char c)
{
    if (c != '"')
        key_ += c;
    else
        state_ = VDFState::LookingForValue;
}

void VDF::ReadValue(char c)
{
    if (c != '"')
        value_ += c;
    else
    {
        state_ = VDFState::LookingForKey;
        AddKeyValuePair();
    }
}

bool VDF::CanPop()
{
    return hierarchy_.size() > 0;
}
