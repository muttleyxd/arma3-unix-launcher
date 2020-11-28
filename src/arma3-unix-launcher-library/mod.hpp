#pragma once

#include <filesystem>
#include <map>
#include <string>

#include <fmt/format.h>

class Mod
{
    public:
        Mod(std::filesystem::path const &path);

        std::filesystem::path path_;
        std::map<std::string, std::string> KeyValue;

        std::string GetName() const;
        void LoadAllCPP();
        void LoadFromText(std::string const &text, bool append = false);
        bool IsWorkshopMod(std::filesystem::path const &workshop_path) const;

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

        bool operator==(Mod const &other) const
        {
            return other.path_ == path_ && other.KeyValue == KeyValue;
        }

        operator std::string() const
        {
            std::string out_string = "Path: " + path_.string() + "\n";
            for (const auto &[key, value] : KeyValue)
                out_string += "Key: " + key + " Value: " + value + "\n";
            return out_string;
        }

        friend ::std::ostream &operator<<(::std::ostream &os, Mod const &mod)
        {
            return os << std::string(mod);
        }

    private:
        std::string RemoveWhitespacesAndComments(std::string const &text);
        void ParseCPP(std::string const &text);
};
