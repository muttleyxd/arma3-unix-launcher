#pragma once

#include <map>
#include <string>
#include <string_view>
#include <vector>

class VDF
{
    public:
        VDF() noexcept : state_(VDFState::LookingForKey) {};

        void LoadFromText(std::string_view const text, bool append = false);
        std::vector<std::string> GetValuesWithFilter(std::string_view const filter);

        std::map<std::string, std::string> KeyValue;

    private:
        void AddKeyValuePair();
        std::string RemoveWhitespaces(std::string_view const text);
        void ParseVDF(std::string const &text);
        void ProcessChar(char c);
        void LookForKey(char c);
        void LookForValue(char c);
        void ReadKey(char c);
        void ReadValue(char c);
        bool CanPop();

        enum class VDFState
        {
            LookingForKey,
            LookingForValue,
            ReadingKey,
            ReadingValue,
            QuoteOrBracketExpectedError,
            MissingBracketAtEndError
        };
        VDFState state_;
        std::string key_;
        std::string value_;
        std::vector<std::string> hierarchy_;
};
