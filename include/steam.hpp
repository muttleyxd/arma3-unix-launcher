#ifndef STEAM_HPP_
#define STEAM_HPP_

#include <filesystem>
#include <string>
#include <vector>

class Steam
{
    public:
        Steam(std::vector<std::filesystem::path> search_paths = {"$HOME/.local/share/Steam", "$HOME/.steam/steam"});

        const std::filesystem::path &GetSteamPath() noexcept;
        std::vector<std::filesystem::path> GetInstallPaths();
        std::filesystem::path GetWorkshopPath(std::string const &appid);

    private:
        std::filesystem::path steam_path_;
        std::filesystem::path config_path_ = "config/config.vdf";
};

#endif
