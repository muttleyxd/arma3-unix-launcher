#ifndef __MOD_HPP
#define __MOD_HPP

#include <map>
#include <string>

class Mod
{
    public:
        Mod(const std::string &path = "");

        std::map<std::string, std::string> KeyValue;

        void LoadFromFile(const std::string &path, bool append = false);
        void LoadFromText(const std::string &text, bool append = false);

    private:
        std::string RemoveWhitespacesAndComments(const std::string &text);
        void ParseCPP(const std::string &text);
};

#endif
