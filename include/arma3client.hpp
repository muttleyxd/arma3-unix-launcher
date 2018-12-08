#ifndef ARMA3Client_HPP_
#define ARMA3Client_HPP_

#include <string>
#include <vector>

#include "mod.hpp"

#ifdef __linux
    #define EXECUTABLE_NAME "arma3.x86_64"
#else
    #define EXECUTABLE_NAME "ArmA3.app"
#endif

class ARMA3Client
{
    public:
        ARMA3Client(std::string arma_path, std::string target_workshop_path, bool skip_initialization = false);

        bool CreateSymlinkToWorkshop();
        bool CreateArmaCfg(const std::vector<Mod> &mod_list);
        bool RefreshMods();
        bool Start(const std::string &arguments);

        std::vector<Mod> mods_custom_;
        std::vector<Mod> mods_home_;
        std::vector<Mod> mods_workshop_;

    private:
        void AddModsFromDirectory(std::string dir, std::vector<Mod> &target);
        std::string PickModName(const Mod &mod, const std::vector<std::string> &names);

        std::string path_;
        std::string path_executable_;
        std::string path_workshop_local_;
        std::string path_workshop_target_;

        const std::string symlink_workshop_name_ = "!workshop";
        const std::string symlink_custom_name_ = "!custom";
        const std::vector<std::string> exclusions{"Addons", "dupa"};

        const std::string appid = "107410";

        const std::string executable_name_ = EXECUTABLE_NAME;
};

#endif
