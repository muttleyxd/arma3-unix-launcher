/*
 * Filesystem.cpp
 *
 *  Created on: 14 Oct 2016
 *      Author: muttley
 */

#include "Filesystem.h"

#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdlib>

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstring>

#include "VDF.h"
#include "Utils.h"
#include "Logger.h"

using namespace std;

namespace Filesystem
{
    std::string FILE_NOT_OPEN = "FILE_NOT_OPEN";
    std::string DIR_NOT_FOUND = "DIR_NOT_FOUND";
    std::string NOT_A_SYMLINK = "NOT_A_SYMLINK";

#ifdef __APPLE__
    std::string LocalSharePrefix = "/Library/Application Support";
    std::string BohemiaInteractivePrefix = "/com.vpltd.Arma3";
    std::string SteamPath = LocalSharePrefix + "/Steam";
    std::string LauncherSettingsDirectory = LocalSharePrefix + "/a3unixlauncher";
#else
    std::string LocalSharePrefix = "/.local/share";
    std::string BohemiaInteractivePrefix = "/bohemiainteractive/arma3";
    std::string SteamPath = "/.steam/steam";
    std::string LauncherSettingsDirectory = "/.config/a3unixlauncher";
#endif
    std::string HomeDirectory = getenv("HOME");

    std::string SteamConfigFile = HomeDirectory + LocalSharePrefix + "/Steam/config/config.vdf";
    std::string SteamConfigFileNeon = HomeDirectory + "/.steam/steam/config/config.vdf";
    std::string SteamAppsArmaPath = "/steamapps/common/Arma 3";
    std::string SteamAppsModWorkshopPath = "/steamapps/workshop/content/107410";

    std::string LauncherSettingsFilename = "/settings.conf";

    std::string ArmaDirWorkshop = "/!workshop";
    std::string ArmaDirCustom = "/!custom";
    std::string ArmaDirDoNotChange = "/!DO_NOT_CHANGE_FILES_IN_THESE_FOLDERS";
    std::string ArmaDirMark = "~arma";

    std::string ArmaConfigFile = HomeDirectory + LocalSharePrefix + BohemiaInteractivePrefix +
                                 "/GameDocuments/Arma 3/Arma3.cfg";

    std::vector<std::string> GetSteamLibraries()
    {
        vector<string> response;
        string steamConfigFile;
        ifstream configFile;

        std::string config_file_path = SteamConfigFile;
        if (!FileExists(config_file_path))
            config_file_path = SteamConfigFileNeon;

        configFile.open(config_file_path, ios::in);
        if (configFile.is_open())
        {
            LOG(0, "File " + SteamConfigFile + " successfully opened...\nReading libraries list...\n");
            getline(configFile, steamConfigFile, '\0');
            VDF vdfReader(steamConfigFile);
            string currentLibraryPath = "";
            int libraryNumber = 1;

            while (true)
            {
                currentLibraryPath = vdfReader.GetValue("InstallConfigStore/Software/Valve/Steam/BaseInstallFolder_" + to_string(
                        libraryNumber++));
                if (currentLibraryPath == KEY_NOT_FOUND)
                    break;
                response.push_back(currentLibraryPath);
            }
        }
        else
            LOG(1, "Can't open " + SteamConfigFile + "\nCritical error\n");

        string home_steam_dir = HomeDirectory + LocalSharePrefix + "/Steam/steamapps/common";
        if (DirectoryExists(home_steam_dir))
            response.push_back(home_steam_dir);
        return response;
    }

    std::string GetDirectory(DirectoryToFind dtf)
    {
        string DirName = "";
        switch (dtf)
        {
            case DirectoryToFind::ArmaInstall:
                DirName = SteamAppsArmaPath;
                break;
            case DirectoryToFind::WorkshopMods:
                DirName = SteamAppsModWorkshopPath;
                break;
            default:
                DirName = SteamAppsModWorkshopPath;
                break;
        }
        DIR *dir = opendir((HomeDirectory + SteamPath + DirName).c_str());
        if (dir)
        {
            closedir(dir);
            return HomeDirectory + SteamPath + DirName;
        }
        for (string s : GetSteamLibraries())
        {
            DIR *dir = opendir((s + DirName).c_str());
            if (dir)
            {
                closedir(dir);
                return s + DirName;
            }
        }
        return DIR_NOT_FOUND;
    }

    bool FileExists(string path)
    {
        struct stat buffer;
        return (stat(path.c_str(), &buffer) == 0);
    }

    bool DirectoryExists(string path)
    {
        struct stat buffer;
        int errorCode = stat(path.c_str(), &buffer);
        if (errorCode == -1)
        {
            if (errno == ENOENT)
                return false;
            LOG(1, "Stat Error in DirectoryExists(" + path + ");");
            return false;
        }
        else
        {
            if (S_ISDIR(buffer.st_mode))
                return true;
            return false;
        }
    }

    bool WriteAllText(string path, string value)
    {
        LOG(0, "Writing to file " + path);
        ofstream outFile(path);
        if (outFile.is_open())
        {
            LOG(0, "File write success");
            outFile << value;
            outFile.close();
            return true;
        }
        LOG(1, "Can't open file " + path + " for write");
        return false;
    }

    string ReadAllText(string path, bool suppress_log)
    {
        if (!suppress_log)
            LOG(0, "Reading file " + path + "... ");
        string response;
        ifstream inFile;
        inFile.open(path, ios::in);
        if (inFile.is_open())
        {
            getline(inFile, response, '\0');
            inFile.close();
            if (!suppress_log)
                LOG(0, "File read successfully");
            return response;
        }
        if (!suppress_log)
            LOG(1, "Can't open file " + path + " for read");
        return FILE_NOT_OPEN;
    }

    vector<Mod> FindMods(string path)
    {
        vector<Mod> response;

        for (string s : GetSubDirectories(path))
        {
            if (s == "Curator" || s == "Dta" || s == "Expansion" || s == "Heli"
                    || s == "Jets" || s == "Kart" || s == "Mark" || s == "Argo"
                    || s == "Orange" || s == "Tacops" || s == "Tank") //skip DLCs
                continue;
            if (Utils::ContainsAddons(path + "/" + s) && Utils::ContainsCppFile(path + "/" + s))
            {
                if (isdigit(s.at(0)))
                    response.push_back(Mod(path + "/" + s, s));
                else
                    response.push_back(Mod(path + "/" + s, "-1"));
            }
        }
        return response;
    }

    void CheckFileStructure(string armaDir, string workshopDir, vector<Mod> modList)
    {
        LOG(0, "Checking file structure");
        string armaDirWorkshopPath = armaDir + Filesystem::ArmaDirWorkshop;
        string armaDirCustomPath = armaDir + Filesystem::ArmaDirCustom;

        LOG(0, "!workshop -> " + armaDirWorkshopPath + "\n!custom -> " + armaDirCustomPath);

        if (!DirectoryExists(armaDirWorkshopPath))
        {
            if (!CreateDirectory(armaDirWorkshopPath))
                return;
        }

        if (!DirectoryExists(armaDirWorkshopPath + Filesystem::ArmaDirDoNotChange))
        {
            if (!CreateDirectory(armaDirWorkshopPath + Filesystem::ArmaDirDoNotChange))
                return;
        }

        if (!DirectoryExists(armaDirCustomPath))
        {
            if (!CreateDirectory(armaDirCustomPath))
                return;
        }

        if (!DirectoryExists(armaDirCustomPath + Filesystem::ArmaDirDoNotChange))
        {
            if (!CreateDirectory(armaDirCustomPath + Filesystem::ArmaDirDoNotChange))
                return;
        }

        vector<string> ModDirs = GetSubDirectories(armaDirWorkshopPath);
        CheckSymlinks(armaDirWorkshopPath, armaDir, workshopDir, &ModDirs, &modList);

        ModDirs = GetSubDirectories(armaDirCustomPath);
        CheckSymlinks(armaDirCustomPath, armaDir, workshopDir, &ModDirs, &modList);

        for (Mod m : modList)
        {
            if (m.Path.find(armaDir) != string::npos)
                continue;
            if (!m.IsRepresentedBySymlink)
            {
                string linkName = armaDirWorkshopPath + "/@" + m.DirName;
                if (m.WorkshopId == "-1")
                    linkName = armaDirCustomPath + "/@" + m.DirName;
                if (!DirectoryExists(linkName) || !FileExists(linkName))
                {
                    int result = symlink(m.Path.c_str(), linkName.c_str());
                    if (result != 0)
                        LOG(1, "Symlink creation failed: " + m.Path + "->" + linkName);
                    else
                        LOG(0, "Symlink creation success: " + m.Path + "->" + linkName);
                }
                else
                    LOG(1, "Dir/file " + linkName + " already exists");
            }
        }
    }

    vector<string> GetSubDirectories(string path)
    {
        vector<string> response;
        struct dirent *directoryEntry;

        DIR *pathDir = opendir(path.c_str());

        if (pathDir == NULL)
        {
            LOG(1, "Opening directory " + path + " failed");
            return response;
        }

        while ((directoryEntry = readdir(pathDir)) != NULL)
        {
            struct stat st;

            if ((strcmp(directoryEntry->d_name, ".") == 0) || (strcmp(directoryEntry->d_name, "..") == 0))
                continue;

            if (fstatat(dirfd(pathDir), directoryEntry->d_name, &st, 0) < 0)
            {
                int error = errno;
                string DirName = directoryEntry->d_name;
                DirName += " strerror: ";
                DirName += strerror(errno);
                LOG(1, "Directory error " + DirName);
                continue;
            }

            if (S_ISDIR(st.st_mode))
                response.push_back(directoryEntry->d_name);
        }

        closedir(pathDir);

        return response;
    }

    bool CreateDirectory(string path)
    {
        int status = mkdir(path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH); //d rwx r-x r-x
        if (status != 0)
        {
            LOG(1, "Can't create directory " + path);
            return false;
        }
        LOG(0, "Successfully created directory " + path);
        return true;
    }

    string GetSymlinkTarget(string path)
    {
        struct stat statinfo;
        if (lstat(path.c_str(), &statinfo) < 0)
        {
            LOG(0, "Can't open directory/symlink " + path);
            return NOT_A_SYMLINK;
        }
        if (S_ISLNK(statinfo.st_mode))
        {
            char *buffer = new char[PATH_MAX + 1];
            size_t pathLength = readlink(path.c_str(), buffer, PATH_MAX);
            buffer[pathLength] = '\0';
            string target = buffer;
            delete[] buffer;
            LOG(0, "!workshop/" + path + " points to: " + target);

            return target;
        }
        return NOT_A_SYMLINK;
    }

    void CheckSymlinks(std::string path, std::string armaDir, std::string workshopDir, vector<string> *ModDirs,
                       vector<Mod> *modList)
    {
        for (int i = 0; i < ModDirs->size(); i++)
        {
            string s = ModDirs->operator [](i);
            string target = GetSymlinkTarget(path + "/" + s);
            if (target != NOT_A_SYMLINK)
            {
                int targetLength = target.length();
                target = Utils::Replace(target, workshopDir + "/", "");

                //outside workshop dir
                if (targetLength == target.length())
                {
                    for (int i = 0; i < modList->size(); i++)
                    {
                        if (modList->operator[](i).Path == target)
                        {
                            modList->operator[](i).IsRepresentedBySymlink = true;
                            break;
                        }
                    }
                }
                else
                {
                    string newWorkshopId = target;
                    for (int i = 0; i < modList->size(); i++)
                    {
                        if (modList->operator[](i).WorkshopId == newWorkshopId && s == modList->operator[](i).Name)
                        {
                            modList->operator[](i).IsRepresentedBySymlink = true;
                            break;
                        }
                    }
                }
            }
            else
                LOG(1, "Not a symlink found in ModDirs!");
        }
    }

    string GenerateArmaCfg(string armaPath, string source, vector<Mod *> modList)
    {
        LOG(1, "Generating Arma3.cfg");
        string response;
        string inputFile = ReadAllText(source);
        if (inputFile == FILE_NOT_OPEN) {
            inputFile = "";
            std::string command = "mkdir -p $(realpath -m $(dirname \"" + source + "\"))";
            LOG(1, "Cannot read Arma3.cfg, creating directory " + command);
            system(command.c_str());
        }

        string modLauncherList = "class ModLauncherList\n{\n";
        int i;

        for (i = 0; i < modList.size(); i++)
        {
            string fullPath = armaPath;
            string symlinkAt = "@";
            if (modList[i]->WorkshopId == "-1")
            {
                if (fullPath.find(armaPath) == std::string::npos)
                    fullPath += Filesystem::ArmaDirCustom;
                else
                    symlinkAt = "";
            }
            else
                fullPath += Filesystem::ArmaDirWorkshop;
            fullPath += "/" + symlinkAt + modList[i]->DirName;
            string testPath = GetSymlinkTarget(fullPath);
            if (testPath != NOT_A_SYMLINK) {
                fullPath = testPath;
            };
            string dirName = symlinkAt + modList[i]->DirName;

            string disk = "C:";
            if (IsProton(GetDirectory(DirectoryToFind::ArmaInstall)))
                disk = "Z:";
            string windowsPath = disk + Utils::Replace(fullPath, "/", "\\");
            modLauncherList += "\tclass Mod" + to_string(i + 1) + "\n\t{"
                               + "\n\t\tdir=\"" + dirName + "\";"
                               + "\n\t\tname=\"" + modList[i]->Name + "\";"
                               + "\n\t\torigin=\"GAME DIR\";"
                               + "\n\t\tfullPath=\"" + windowsPath + "\";"
                               + "\n\t};\n";
        }

        modLauncherList += "};";

        int leftBracketsOpen = 0;
        bool ignoreAll = false;

        vector<string> lines = Utils::Split(inputFile, "\n");
        for (string s : lines)
        {
            string logMsg = "Line: " + s + "\n";
            if (Utils::Trim(s) == "{")
            {
                leftBracketsOpen++;
                logMsg += "Left brackets opened: " + to_string(leftBracketsOpen);
            }
            else if (Utils::Trim(s) == "};")
            {
                leftBracketsOpen--;
                if (leftBracketsOpen == 0)
                    ignoreAll = false;
                logMsg += "Left brackets opened: " + to_string(leftBracketsOpen)
                          + "\nignoreAll = " + Utils::ToString(ignoreAll);
            }
            else
            {
                if (Utils::Trim(s) == "class ModLauncherList")
                    ignoreAll = true;
                if (!ignoreAll)
                    response += s + "\n";
                logMsg += "\nignoreAll = " + Utils::ToString(ignoreAll);
            }
            LOG(0, logMsg);
        }
        response += modLauncherList + "\n";
        return response;
    }

    bool IsProton(std::string path)
    {
        return Filesystem::FileExists(path + "/arma3launcher.exe");
    }
}
