#ifndef ARMA3Client_HPP_
#define ARMA3Client_HPP_

#include <filesystem>
#include <string>
#include <vector>

#include "mod.hpp"

namespace ARMA3::Definitions
{
    static std::string home_directory = getenv("HOME");

    static constexpr const char *app_id = "107410";

    static const std::array<const char *, 20> exclusions{"!custom", "!workshop", "Addons", "Argo", "BattlEye", "Curator", "Dta", "Expansion", "Heli", "Jets", "Kart", "Keys", "MPMissions", "Mark", "Missions", "Orange", "Tacops", "Tank", "legal", "steam_shader_cache"};

    static constexpr const char *symlink_workshop_name = "!workshop";
    static constexpr const char *symlink_custom_name = "!custom";
    static constexpr const char *do_not_change_name = "!DO_NOT_CHANGE_FILES_IN_THESE_FOLDERS";

#ifdef __linux
    static constexpr const char *executable_name = "arma3.x86_64";
    static constexpr const char *local_share_prefix = ".local/share";
    static constexpr const char *bohemia_interactive_prefix = "bohemiainteractive/arma3";
    static constexpr const char *game_config_path = "GameDocuments/Arma 3/Arma3.cfg";
#else //__APPLE__
    static constexpr const char *executable_name "ArmA3.app"
    static constexpr const char *local_share_prefix = "Library/Application Support";
#endif
}

namespace ARMA3
{
    class Client
    {
        public:
            Client(std::filesystem::path const &arma_path, std::filesystem::path const &target_workshop_path, bool skip_initialization = false);

            bool CreateSymlinkToWorkshop();
            void CreateArmaCfg(std::vector<Mod> const &mod_list, std::filesystem::path cfg_path);
            bool RefreshMods();
            void Start(std::string const &arguments);
            void AddCustomMod(std::filesystem::path const &path);

            std::vector<Mod> mods_custom_;
            std::vector<Mod> mods_home_;
            std::vector<Mod> mods_workshop_;

        private:
            void AddModsFromDirectory(std::filesystem::path const &dir, std::vector<Mod> &target);
            std::string PickModName(Mod const &mod, std::vector<std::string> const &names);
            std::filesystem::path GetCfgPath();

            std::filesystem::path path_;
            std::filesystem::path path_custom_;
            std::filesystem::path path_executable_;
            std::filesystem::path path_workshop_local_;
            std::filesystem::path path_workshop_target_;
    };
}

#endif
