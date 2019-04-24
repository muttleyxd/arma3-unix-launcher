/*
 * Filesystem.h
 *
 *  Created on: 14 Oct 2016
 *      Author: muttley
 */

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <string>
#include <vector>
#include <map>
#include "Mod.h"

enum DirectoryToFind
{
    ArmaInstall = 0,
    WorkshopMods = 1
};

namespace Filesystem
{
    extern std::string FILE_NOT_OPEN;
    extern std::string DIR_NOT_FOUND;
    extern std::string NOT_A_SYMLINK;

    extern std::string LocalSharePrefix;
    extern std::string BohemiaInteractivePrefix;
    extern std::string SteamPath;
    extern std::string LauncherSettingsDirectory;

    extern std::string SteamConfigFile;
    extern std::string SteamConfigFileNeon;
    extern std::string SteamAppsArmaPath;
    extern std::string SteamAppsModWorkshopPath;
    extern std::string SteamPath;
    extern std::string HomeDirectory;
    extern std::string LauncherSettingsDirectory;
    extern std::string LauncherSettingsFilename;

    extern std::string ArmaDirWorkshop;
    extern std::string ArmaDirCustom;

    extern std::string ArmaDirDoNotChange;

    extern std::string ArmaDirMark;

    extern std::string ArmaConfigFile;

    //profiles - *.profile files in settings directory

    std::vector<std::string> GetSteamLibraries();
    std::string GetDirectory(DirectoryToFind dtf);

    bool FileExists(std::string path);
    bool DirectoryExists(std::string path);

    bool WriteAllText(std::string path, std::string value);
    std::string ReadAllText(std::string path, bool suppress_log = false);

    std::vector<Mod> FindMods(std::string path);

    void CheckFileStructure(std::string armaDir, std::string workshopDir, std::vector<Mod> modList);

    std::vector<std::string> GetSubDirectories(std::string path);

    bool CreateDirectory(std::string path);

    std::string GetSymlinkTarget(std::string path);
    void CheckSymlinks(std::string path, std::string armaDir, std::string workshopDir, std::vector<std::string> *ModDirs,
                       std::vector<Mod> *modList);

    std::string GenerateArmaCfg(std::string armaPath, std::string source, std::vector<Mod *> modList);

    bool IsProton(std::string path);
}
#endif /* FILESYSTEM_H_ */
