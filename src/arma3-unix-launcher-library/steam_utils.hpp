#pragma once

#include <filesystem>
#include <string>
#include <vector>

class SteamUtils
{
    public:
        SteamUtils(std::vector<std::filesystem::path> search_paths = {"$HOME/.local/share/Steam", "$HOME/.steam/steam", "$HOME/Library/Application Support/Steam"});

        std::filesystem::path const &GetSteamPath() const noexcept;
        std::vector<std::filesystem::path> GetInstallPaths() const;
        std::filesystem::path GetGamePathFromInstallPath(std::filesystem::path const &install_path,
                std::string const &appid) const;
        std::filesystem::path GetWorkshopPath(std::filesystem::path const &install_path, std::string const &appid) const;
        std::pair<std::filesystem::path, std::string> GetCompatibilityToolForAppId(std::uint64_t const app_id);
        std::filesystem::path GetInstallPathFromGamePath(std::filesystem::path const &game_path);

    private:
        std::filesystem::path steam_path_;
        std::filesystem::path config_path_ = "config/config.vdf";
};
