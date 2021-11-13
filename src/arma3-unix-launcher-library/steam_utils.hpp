#pragma once

#include <filesystem>
#include <string>
#include <vector>

class SteamUtils
{
    public:
        SteamUtils(std::vector<std::filesystem::path> const &search_paths = {"$HOME/.steam/steam", "$HOME/.local/share/Steam", "$HOME/.var/app/com.valvesoftware.Steam/.steam/steam", "$HOME/.var/app/com.valvesoftware.Steam/.local/share/Steam", "$HOME/Library/Application Support/Steam"});

        std::filesystem::path const &GetSteamPath() const noexcept;
        std::vector<std::filesystem::path> GetInstallPaths() const;
        std::filesystem::path GetGamePathFromInstallPath(std::filesystem::path const &install_path,
                std::string_view const appid) const;
        std::filesystem::path GetWorkshopPath(std::filesystem::path const &install_path, std::string const &appid) const;
        std::pair<std::filesystem::path, std::string> GetCompatibilityToolForAppId(std::uint64_t const app_id) const;
        std::filesystem::path GetInstallPathFromGamePath(std::filesystem::path const &game_path) const;
        bool IsFlatpak() const;

    private:
        std::filesystem::path get_compatibility_tool_path(std::string const &shortname) const;
        std::filesystem::path get_builtin_compatibility_tool(std::string const &shortname,
                std::string_view const app_id_str) const;

        std::filesystem::path steam_path_;
        std::filesystem::path config_path_ = "config/config.vdf";
        std::filesystem::path library_folders_path_ = "config/libraryfolders.vdf";

        std::filesystem::path get_user_compatibility_tool(const std::string &shortname) const;
};
