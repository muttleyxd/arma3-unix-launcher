#ifndef MOD_HPP_
#define MOD_HPP_

#include <map>
#include <string>

class Mod
{
    public:
        std::string path_;
        std::map<std::string, std::string> KeyValue;

        void LoadAllCPP();
        void LoadFromFile(const std::string &path, bool append = false);
        void LoadFromText(const std::string &text, bool append = false);

        bool operator==(const Mod &other) const
        {
            return other.path_ == path_ && other.KeyValue == KeyValue;
        }

        friend ::std::ostream &operator<<(::std::ostream &os, const Mod &mod)
        {
            std::string out_string = mod.path_ + "\n";
            for (const auto &[key, value] : mod.KeyValue)
                out_string += "Key: " + key + " Value: " + value + "\n";
            return os << out_string;
        }

    private:
        std::string RemoveWhitespacesAndComments(const std::string &text);
        void ParseCPP(const std::string &text);
};

#endif
