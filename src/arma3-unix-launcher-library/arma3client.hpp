#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "mod.hpp"

namespace ARMA3::Definitions
{
    static std::string home_directory = getenv("HOME");

    static constexpr char const *app_id = "107410";

    static const std::array<char const *, 23> exclusions{"Addons", "Argo", "BattlEye", "Contact", "Curator", "Dll", "Dta", "Enoch", "Expansion", "fontconfig", "Heli", "Jets", "Kart", "Keys", "Launcher", "MPMissions", "Mark", "Missions", "Orange", "Tacops", "Tank", "legal", "steam_shader_cache"};

    #ifdef __linux
    static constexpr std::array<char const *, 2> const executable_names {"arma3.x86_64", "arma3_x64.exe"};
    static constexpr char const *local_share_prefix = ".local/share";
    static constexpr char const *bohemia_interactive_prefix = "bohemiainteractive/arma3";
    static constexpr char const *game_config_path = "GameDocuments/Arma 3/Arma3.cfg";
    #else //__APPLE__
    static constexpr std::array<char const *, 1> const executable_names {"ArmA3.app"};
    static constexpr char const *local_share_prefix = "Library/Application Support";
    static constexpr char const *bohemia_interactive_prefix = "com.vpltd.Arma3";
    static constexpr char const *game_config_path = "GameDocuments/Arma 3/Arma3.cfg";
    #endif

    static constexpr char const *proton_config_relative_path =
        "../../compatdata/107410/pfx/drive_c/users/steamuser/My Documents/Arma 3/Arma3.cfg";
}

namespace ARMA3
{
    class Client
    {
        public:
            Client(std::filesystem::path const &arma_path, std::filesystem::path const &target_workshop_path);

            void CreateArmaCfg(std::vector<std::string> const &workshop_mod_ids,
                               std::vector<std::filesystem::path> const &custom_mods, std::filesystem::path cfg_path = "");
            bool IsProton();
            void Start(std::string const &arguments);

            std::vector<Mod> GetHomeMods();
            std::vector<Mod> GetWorkshopMods();

            std::filesystem::path const &GetPath();
            std::filesystem::path const &GetPathExecutable();
            std::filesystem::path const &GetPathWorkshop();

        private:
            std::vector<Mod> GetModsFromDirectory(std::filesystem::path const &dir);
            std::filesystem::path GetCfgPath();
            char GetFakeDriveLetter();
            std::string GenerateCfgCppForMod(std::filesystem::path const &path, int mod_index);

            std::filesystem::path path_;
            std::filesystem::path path_custom_;
            std::filesystem::path path_executable_;
            std::filesystem::path path_workshop_local_;
            std::filesystem::path path_workshop_target_;
    };
}
