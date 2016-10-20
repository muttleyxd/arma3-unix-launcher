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
#include "Mod.h"

enum DirectoryToFind
{
	ArmaInstall = 0,
	WorkshopMods = 1
};

class Filesystem
{
	public:
		Filesystem();
		~Filesystem();

		std::string STEAM_CONFIG_FILE = "/.local/share/Steam/config/config.vdf";
		std::string SteamAppsArmaPath = "/steamapps/common/Arma 3";
		std::string SteamAppsModWorkshopPath = "/steamapps/workshop/content/107410";
		std::string SteamPath = "/.steam/steam";
		std::string HomeDirectory = getenv("HOME");
		std::string LauncherSettingsDirectory = "/.config/a3linuxlauncher";
		std::string LauncherSettingsFilename = "/settings.conf";

		//profiles - *.profile files in settings directory

		std::vector<std::string> GetSteamLibraries();
		std::string GetDirectory(DirectoryToFind dtf);

		bool FileExists(std::string path);
		bool DirectoryExists(std::string path);

		void WriteAllText(std::string path, std::string value);
		std::string ReadAllText(std::string path);

		std::vector<Mod> FindMods(std::string path);

		void CheckFileStructure(std::string armaDir, std::string workshopDir, std::vector<Mod> modList);

		std::vector<std::string> GetSubDirectories(std::string path);

		bool CreateDirectory(std::string path);
};

#endif /* FILESYSTEM_H_ */
