#ifndef __VDF_HPP
#define __VDF_HPP

#include "vdfkey.hpp"

#include <map>
#include <string>
#include <vector>

class VDF
{
    public:
        VDF() noexcept;

        void LoadFromFile(const std::string &path, bool append = false);
        void LoadFromText(const std::string &text, bool append = false);

        std::vector<std::string> GetValuesWithFilter(std::string filter);

        std::map<std::string, std::string> KeyValue;

    private:
        void AddKeyValuePair(std::vector<std::string> &hierarchy, std::string &key, std::string &value);
        std::string RemoveWhitespaces(const std::string &text);
        void ParseVDF(const std::string &text);
        void ProcessChar(char c);
        void LookForKey(char c);
        void LookForValue(char c);
        void ReadKey(char c);
        void ReadValue(char c);

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
        std::string key;
        std::string value;
        std::vector<std::string> hierarchy;
};

#endif
