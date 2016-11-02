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

	extern std::string STEAM_CONFIG_FILE;
	extern std::string SteamAppsArmaPath;
	extern std::string SteamAppsModWorkshopPath;
	extern std::string SteamPath;
	extern std::string HomeDirectory;
	extern std::string LauncherSettingsDirectory;
	extern std::string LauncherSettingsFilename;
	extern std::string LauncherCustomModDirectory;

	extern std::string ArmaDirMark;

	//profiles - *.profile files in settings directory

	std::vector<std::string> GetSteamLibraries();
	std::string GetDirectory(DirectoryToFind dtf);

	bool FileExists(std::string path);
	bool DirectoryExists(std::string path);

	bool WriteAllText(std::string path, std::string value);
	std::string ReadAllText(std::string path);

	std::vector<Mod> FindMods(std::string path);

	void CheckFileStructure(std::string armaDir, std::string workshopDir, std::vector<Mod> modList);

	std::vector<std::string> GetSubDirectories(std::string path);

	bool CreateDirectory(std::string path);
};

#endif /* FILESYSTEM_H_ */
