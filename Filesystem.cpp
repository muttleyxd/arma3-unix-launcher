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

	std::string STEAM_CONFIG_FILE = "/.local/share/Steam/config/config.vdf";
	std::string SteamAppsArmaPath = "/steamapps/common/Arma 3";
	std::string SteamAppsModWorkshopPath = "/steamapps/workshop/content/107410";
	std::string SteamPath = "/.steam/steam";
	std::string HomeDirectory = getenv("HOME");
	std::string LauncherSettingsDirectory = "/.config/a3linuxlauncher";
	std::string LauncherSettingsFilename = "/settings.conf";

	std::string ArmaDirWorkshop = "/!workshop";
	std::string ArmaDirCustom = "/!custom";

	std::string ArmaDirDoNotChange = "/!DO_NOT_CHANGE_FILES_IN_THESE_FOLDERS";

	std::string ArmaDirMark = "~arma";

	std::string ArmaConfigFile = "/.local/share/bohemiainteractive/arma3/GameDocuments/Arma 3/Arma3.cfg";

	std::vector<std::string> GetSteamLibraries()
	{
		vector<string> response;
		string steamConfigFile;
		ifstream configFile;
		configFile.open(HomeDirectory + STEAM_CONFIG_FILE, ios::in);
		if (configFile.is_open())
		{
			LOG(0, "File " + HomeDirectory + STEAM_CONFIG_FILE + " successfully opened...\nReading libraries list...\n");
			getline(configFile, steamConfigFile, '\0');
			VDF vdfReader(steamConfigFile);
			string currentLibraryPath = "";
			int libraryNumber = 1;

			while (true)
			{
				currentLibraryPath = vdfReader.GetValue("InstallConfigStore/Software/Valve/Steam/BaseInstallFolder_" + to_string(libraryNumber++));
				if (currentLibraryPath == KEY_NOT_FOUND)
					break;
				response.push_back(currentLibraryPath);
			}
		}
		else
			LOG(1, "Can't open " + HomeDirectory + STEAM_CONFIG_FILE + "\nCritical error\n");
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
		DIR* dir = opendir((SteamPath + DirName).c_str());
		if (dir)
		{
			closedir(dir);
			return SteamPath + DirName;
		}
		for (string s: GetSteamLibraries())
		{
			DIR* dir = opendir((s + DirName).c_str());
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

	string ReadAllText(string path)
	{
		LOG(0, "Reading file " + path + "... ");
		string response;
		ifstream inFile;
		inFile.open(path, ios::in);
		if (inFile.is_open())
		{
			getline(inFile, response, '\0');
			inFile.close();
			LOG(0, "File read successfully");
			return response;
		}
		LOG(1, "Can't open file " + path + " for read");
		return FILE_NOT_OPEN;
	}

	vector<Mod> FindMods(string path)
	{
		vector<Mod> response;

		for (string s: GetSubDirectories(path))
		{
		    if (s == "curator" || s == "dta" || s == "Expansion" || s == "heli"
		            || s == "kart" || s == "mark") //skip DLCs
		        continue;
			if (DirectoryExists(path + "/" + s + "/Addons")
					|| DirectoryExists(path + "/" + s + "/addons"))
				response.push_back(Mod(path + "/" + s, s));
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

		for (Mod m: modList)
		{
		    if (m.Path.find(armaDir) != string::npos)
		        continue;
			if (!m.IsRepresentedBySymlink)
			{
				string linkName = armaDirWorkshopPath + "/@" + m.Name;
				if (m.WorkshopId == "-1")
				    linkName = armaDirCustomPath + "/@" + m.Name;
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
		struct dirent* directoryEntry;

		DIR* pathDir = opendir(path.c_str());

		if (pathDir == NULL)
		{
			LOG(1, "Opening directory " + path + " failed");
			return response;
		}
		LOG(0, "Opening directory " + path + " succeeded");

		while ((directoryEntry = readdir(pathDir)) != NULL)
		{
			struct stat st;

			if ((strcmp(directoryEntry->d_name, ".") == 0) || (strcmp(directoryEntry->d_name, "..") == 0))
				continue;

			if (fstatat(dirfd(pathDir), directoryEntry->d_name, &st, 0) < 0)
			{
				string DirName = directoryEntry->d_name;
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
            char* buffer = new char[PATH_MAX + 1];
            size_t pathLength = readlink(path.c_str(), buffer, PATH_MAX);
            buffer[pathLength] = '\0';
            string target = buffer;
            delete[] buffer;
            LOG(0, "!workshop/" + path + " points to: " + target);

            return target;
        }
        return NOT_A_SYMLINK;
	}

	void CheckSymlinks(std::string path, std::string armaDir, std::string workshopDir, vector<string>* ModDirs, vector<Mod>* modList)
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
                        if (modList->operator[](i).WorkshopId == newWorkshopId)
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
}
