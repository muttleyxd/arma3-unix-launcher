/*
 * Settings.h
 *
 *  Created on: 14 Oct 2016
 *      Author: muttley
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <vector>
#include <string>

#include "Mod.h"

namespace Settings
{
	extern std::string ArmaPath;
	extern std::string WorkshopPath;
	extern bool ArmaPathAutodetect;
	extern bool WorkshopPathAutodetect;

	extern int WindowSizeX;
	extern int WindowSizeY;
	extern int WindowPosX;
	extern int WindowPosY;

	extern bool SkipIntro;
	extern bool Nosplash;
	extern bool Window;
	extern bool Name;
	extern std::string NameValue;

	extern bool ParameterFile;
	extern std::string ParameterFileValue;

	extern bool CheckSignatures;

	extern bool CpuCount;
	extern int CpuCountValue;

	extern bool ExThreads;
	extern bool ExThreadsFileOperations;
	extern bool ExThreadsTextureLoading;
	extern bool ExThreadsGeometryLoading;

	extern bool EnableHT;
	extern bool FilePatching;
	extern bool NoLogs;

	extern bool World;
	extern std::string WorldValue;

	extern bool Profile;
	extern std::string ProfileValue;

	extern bool NoPause;

	extern bool Connect;
	extern std::string ConnectValue;

	extern bool Port;
	extern std::string PortValue;

	extern bool Password;
	extern std::string PasswordValue;

	extern bool Host;

	bool Load(std::string path);
	bool Save(std::string path);

	std::vector<Mod> LoadUserMods();
	bool SaveUserMods(std::vector<Mod> mods);
};

#endif /* SETTINGS_H_ */
