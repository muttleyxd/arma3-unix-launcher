#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "mod.hpp"

namespace DayZ::Definitions
{
    static std::string home_directory = getenv("HOME");

    static constexpr char const *app_id = "221100";

    static const std::array<char const *, 6> exclusions{"Addons", "BattlEye", "EmptySteamDepot", "Launcher", "bliss", "dta"};
    // TODO: Use dta/product.bin for exclusion list

    static constexpr char const *executable_name = "DayZ_x64.exe";
    static constexpr char const *local_share_prefix = ".local/share";
    static constexpr char const *bohemia_interactive_prefix = "bohemiainteractive/arma3";
    static constexpr char const *game_config_path =
        "../../compatdata/221100/pfx/drive_c/users/steamuser/My Documents/DayZ/DayZ.cfg";
    static constexpr char const *flatpak_prefix = ".var/app/com.valvesoftware.Steam";
}

//TODO: Commonize code between this and a3ul-library
namespace DayZ
{
    class Client
    {
        public:
            Client(std::filesystem::path const &arma_path, std::filesystem::path const &target_workshop_path);

            void Start(std::vector<std::filesystem::path> const &mod_paths, std::string const &arguments,
                       std::string const &user_environment_variables,
                       bool launch_directly = false, bool disable_esync = false);

            std::vector<Mod> GetHomeMods();
            std::vector<Mod> GetWorkshopMods();

            std::filesystem::path const &GetPath();
            std::filesystem::path const &GetPathExecutable();
            std::filesystem::path const &GetPathWorkshop();

        private:
            std::vector<Mod> GetModsFromDirectory(std::filesystem::path const &dir);
            std::filesystem::path GetCfgPath();
            char GetFakeDriveLetter();
            bool IsFlatpak();
            std::string GetModParameter(std::size_t count);

            std::filesystem::path path_;
            std::filesystem::path path_custom_;
            std::filesystem::path path_executable_;
            std::filesystem::path path_workshop_local_;
            std::filesystem::path path_workshop_target_;
    };
}
