#ifndef MOD_HPP_
#define MOD_HPP_

#include <filesystem>
#include <map>
#include <string>

class Mod
{
    public:
        std::string path_;
        std::map<std::string, std::string> KeyValue;

        void LoadAllCPP();
        void LoadFromFile(const std::filesystem::path &path, bool append = false);
        void LoadFromText(const std::string &text, bool append = false);

        std::string GetValueOrReturnDefault(std::string default_value) const
        {
            return default_value;
        }

        template<typename ... Args>
        std::string GetValueOrReturnDefault(std::string key, Args... keys) const
        {
            auto iterator = KeyValue.find(key);
            if (iterator != KeyValue.end())
                return iterator->second;
            return GetValueOrReturnDefault(keys...);
        }

        bool operator==(const Mod &other) const
        {
            return other.path_ == path_ && other.KeyValue == KeyValue;
        }

        operator std::string() const
        {
            std::string out_string = "Path: " + path_ + "\n";
            for (const auto &[key, value] : KeyValue)
                out_string += "Key: " + key + " Value: " + value + "\n";
            return out_string;
        }

        friend ::std::ostream &operator<<(::std::ostream &os, const Mod &mod)
        {
            return os << std::string(mod);
        }

    private:
        std::string RemoveWhitespacesAndComments(const std::string &text);
        void ParseCPP(const std::string &text);
};

#endif
