/*
 * Settings.cpp
 *
 *  Created on: 14 Oct 2016
 *      Author: muttley
 */

#include "Settings.h"

#include <fstream>
#include <cstring>
#include <sstream>
#include <iostream>

#include "Filesystem.h"
#include "Utils.h"
#include "Logger.h"

using namespace std;

namespace Settings
{
    string ArmaPath = Filesystem::DIR_NOT_FOUND;
    string WorkshopPath = Filesystem::DIR_NOT_FOUND;

    int WindowPosX = 0;
    int WindowPosY = 0;
    int WindowSizeX = 800;
    int WindowSizeY = 600;

    bool SkipIntro = false;
    bool Nosplash = false;
    bool Window = false;
    bool Name = false;
    string NameValue = "";

    bool ParameterFile = false;
    string ParameterFileValue = "";

    bool CheckSignatures = false;

    bool CpuCount = false;
    int CpuCountValue = 4;

    bool ExThreads = false;
    bool ExThreadsFileOperations = false;
    bool ExThreadsTextureLoading = false;
    bool ExThreadsGeometryLoading = false;

    bool EnableHT = false;
    bool FilePatching = false;
    bool NoLogs = false;

    bool World = false;
    string WorldValue = "";

    bool NoPause = false;

    bool Connect = false;
    string ConnectValue = "";

    bool Port = false;
    string PortValue = "";

    bool Password = false;
    string PasswordValue = "";

    bool Host = false;

    std::vector<std::string> WorkshopModsEnabled;
    std::vector<std::string> WorkshopModsOrder;

    std::vector<std::string> CustomModsEnabled;
    std::vector<std::string> CustomModsOrder;

    std::vector<std::string> CustomMods;

    bool Load(string path)
    {
        string loadedFile = Filesystem::ReadAllText(path);
        if (loadedFile != Filesystem::FILE_NOT_OPEN)
        {
            WorkshopModsEnabled.clear();
            WorkshopModsOrder.clear();

            CustomModsEnabled.clear();
            CustomModsOrder.clear();

            string currentPath = "";
            for (string line: Utils::Split(loadedFile, "\n"))
            {
                if (Utils::StartsWith(line, "ArmaPath="))
                    ArmaPath = line.substr(9);
                else if (Utils::StartsWith(line, "WorkshopPath="))
                    WorkshopPath = line.substr(13);
                else if (Utils::StartsWith(line, "WindowSizeX="))
                    WindowSizeX = strtol(line.substr(12).c_str(), NULL, 10);
                else if (Utils::StartsWith(line, "WindowSizeY="))
                    WindowSizeY = strtol(line.substr(12).c_str(), NULL, 10);
                else if (Utils::StartsWith(line, "WindowPosX="))
                    WindowPosX = strtol(line.substr(11).c_str(), NULL, 10);
                else if (Utils::StartsWith(line, "WindowPosY="))
                    WindowPosY = strtol(line.substr(11).c_str(), NULL, 10);
                else if (Utils::StartsWith(line, "SkipIntro="))
                    SkipIntro = strtol(line.substr(10).c_str(), NULL, 10);
                else if (Utils::StartsWith(line, "Nosplash="))
                    Nosplash = strtol(line.substr(9).c_str(), NULL, 10);
                else if (Utils::StartsWith(line, "Window="))
                    Window = strtol(line.substr(7).c_str(), NULL, 10);
                else if (Utils::StartsWith(line, "Name="))
                    Name = strtol(line.substr(5).c_str(), NULL, 10);
                else if (Utils::StartsWith(line, "NameValue="))
                    NameValue = line.substr(10);
                else if (Utils::StartsWith(line, "ParameterFile="))
                    ParameterFile = strtol(line.substr(14).c_str(), NULL, 10);
                else if (Utils::StartsWith(line, "ParameterFileValue="))
                    ParameterFileValue = line.substr(19);
                else if (Utils::StartsWith(line, "CheckSignatures="))
                    CheckSignatures = strtol(line.substr(16).c_str(), NULL, 10);
                else if (Utils::StartsWith(line, "CpuCount="))
                    CpuCount = strtol(line.substr(9).c_str(), NULL, 10);
                else if (Utils::StartsWith(line, "CpuCountValue="))
                    CpuCountValue = strtol(line.substr(14).c_str(), NULL, 10);
                else if (Utils::StartsWith(line, "ExThreads="))
                    ExThreads = strtol(line.substr(10).c_str(), NULL, 10);
                else if (Utils::StartsWith(line, "ExThreadsFileOperations="))
                    ExThreadsFileOperations = strtol(line.substr(24).c_str(), NULL, 10);
                else if (Utils::StartsWith(line, "ExThreadsTextureLoading="))
                    ExThreadsTextureLoading = strtol(line.substr(24).c_str(), NULL, 10);
                else if (Utils::StartsWith(line, "ExThreadsGeometryLoading="))
                    ExThreadsGeometryLoading = strtol(line.substr(25).c_str(), NULL, 10);
                else if (Utils::StartsWith(line, "EnableHT="))
                    EnableHT = strtol(line.substr(9).c_str(), NULL, 10);
                else if (Utils::StartsWith(line, "FilePatching="))
                    FilePatching = strtol(line.substr(13).c_str(), NULL, 10);
                else if (Utils::StartsWith(line, "NoLogs="))
                    NoLogs = strtol(line.substr(7).c_str(), NULL, 10);
                else if (Utils::StartsWith(line, "World="))
                    World = strtol(line.substr(6).c_str(), NULL, 10);
                else if (Utils::StartsWith(line, "WorldValue="))
                    WorldValue = line.substr(11);
                else if (Utils::StartsWith(line, "NoPause="))
                    NoPause = strtol(line.substr(8).c_str(), NULL, 10);
                else if (Utils::StartsWith(line, "Connect="))
                    Connect = strtol(line.substr(8).c_str(), NULL, 10);
                else if (Utils::StartsWith(line, "ConnectValue="))
                    ConnectValue = line.substr(13);
                else if (Utils::StartsWith(line, "Port="))
                    Port = strtol(line.substr(5).c_str(), NULL, 10);
                else if (Utils::StartsWith(line, "PortValue="))
                    PortValue = line.substr(10);
                else if (Utils::StartsWith(line, "Password="))
                    Password = strtol(line.substr(9).c_str(), NULL, 10);
                else if (Utils::StartsWith(line, "PasswordValue="))
                    PasswordValue = line.substr(14);
                else if (Utils::StartsWith(line, "Host="))
                    Host = strtol(line.substr(5).c_str(), NULL, 10);
                else if (Utils::StartsWith(line, "WorkshopModsEnabled="))
                {
                    string sub = line.substr(20);
                    for (string s: Utils::Split(sub, ","))
                        WorkshopModsEnabled.push_back(s);
                }
                else if (Utils::StartsWith(line, "WorkshopModsOrder="))
                {
                    string sub = line.substr(18);
                    for (string s: Utils::Split(sub, ","))
                        WorkshopModsOrder.push_back(s);
                }
                else if (Utils::StartsWith(line, "CustomModsEnabled="))
                {
                    string sub = line.substr(18);
                    for (string s: Utils::Split(sub, ","))
                        CustomModsEnabled.push_back(s);
                }
                else if (Utils::StartsWith(line, "CustomModsOrder="))
                {
                    string sub = line.substr(16);
                    for (string s: Utils::Split(sub, ","))
                        CustomModsOrder.push_back(s);
                }
                else
                    LOG(1, "Invalid line in config file " + path);
            }
        }
        else
        {
            LOG(1, "Can't load settings!");
            return false;
        }
        return true;
    }

    bool Save(string path)
    {
        string outFile = "ArmaPath=" + ArmaPath
                        + "\nWorkshopPath=" + WorkshopPath
                        + "\nWindowSizeX=" + to_string(WindowSizeX)
                        + "\nWindowSizeY=" + to_string(WindowSizeY)
                        + "\nWindowPosX=" + to_string(WindowPosX)
                        + "\nWindowPosY=" + to_string(WindowPosY)
                        + "\nSkipIntro=" + Utils::ToString(SkipIntro)
                        + "\nNosplash=" + Utils::ToString(Nosplash)
                        + "\nWindow=" + Utils::ToString(Window)
                        + "\nName=" + Utils::ToString(Name)
                        + "\nNameValue=" + NameValue
                        + "\nParameterFile=" + Utils::ToString(ParameterFile)
                        + "\nParameterFileValue=" + ParameterFileValue
                        + "\nCheckSignatures=" + Utils::ToString(CheckSignatures)
                        + "\nCpuCount=" + Utils::ToString(CpuCount)
                        + "\nCpuCountValue=" + to_string(CpuCountValue)
                        + "\nExThreads=" + Utils::ToString(ExThreads)
                        + "\nExThreadsFileOperations=" + Utils::ToString(ExThreadsFileOperations)
                        + "\nExThreadsTextureLoading=" + Utils::ToString(ExThreadsTextureLoading)
                        + "\nExThreadsGeometryLoading=" + Utils::ToString(ExThreadsGeometryLoading)
                        + "\nEnableHT=" + Utils::ToString(EnableHT)
                        + "\nFilePatching=" + Utils::ToString(FilePatching)
                        + "\nNoLogs=" + Utils::ToString(NoLogs)
                        + "\nWorld=" + Utils::ToString(World)
                        + "\nWorldValue=" + WorldValue
                        + "\nNoPause=" + Utils::ToString(NoPause)
                        + "\nConnect=" + Utils::ToString(Connect)
                        + "\nConnectValue=" + ConnectValue
                        + "\nPort=" + Utils::ToString(Port)
                        + "\nPortValue=" + PortValue
                        + "\nPassword=" + Utils::ToString(Password)
                        + "\nPasswordValue=" + PasswordValue
                        + "\nHost=" + Utils::ToString(Host)
                        + "\nWorkshopModsEnabled=";


        for (string i: WorkshopModsEnabled)
            outFile += i + ",";

        outFile += "\nWorkshopModsOrder=";

        for (string i: WorkshopModsOrder)
            outFile += i + ",";

        outFile += "\nCustomModsEnabled=";

        for (string i: CustomModsEnabled)
            outFile += Utils::Replace(i, Filesystem::ArmaDirMark, Settings::ArmaPath) + ",";

        outFile += "\nCustomModsOrder=";

        for (string i: CustomModsOrder)
            outFile += Utils::Replace(i, Filesystem::ArmaDirMark, Settings::ArmaPath) + ",";

        if (!Filesystem::WriteAllText(path, outFile))
        {
            LOG(1, "Can't write to settings file!");
            return false;
        }
        return true;
    }

    bool ModEnabled(string workshopId)
    {
        if (workshopId.length() == 0)
            return false;
        else if (isdigit(workshopId[0]))
        {
            for (string s: WorkshopModsEnabled)
            {
                if (s == workshopId)
                    return true;
            }
        }
        else
        {
            for (string s: CustomModsEnabled)
            {
                if (s == workshopId)
                    return true;
            }
        }
        return false;
    }
}
