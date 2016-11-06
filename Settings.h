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

    extern bool NoPause;

    extern bool Connect;
    extern std::string ConnectValue;

    extern bool Port;
    extern std::string PortValue;

    extern bool Password;
    extern std::string PasswordValue;

    extern bool Host;

    extern std::vector<std::string> WorkshopModsEnabled;
    extern std::vector<std::string> WorkshopModsOrder;

    extern std::vector<std::string> CustomModsEnabled;
    extern std::vector<std::string> CustomModsOrder;

    bool Load(std::string path);
    bool Save(std::string path);

    bool ModEnabled(std::string workshopId);
};

#endif /* SETTINGS_H_ */
