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
	string ArmaPath = "";
	string WorkshopPath = "";
	int WindowPosX = 0;
	int WindowPosY = 0;
	int WindowSizeX = 800;
	int WindowSizeY = 600;

	bool Load(string path)
	{
		string loadedFile = Filesystem::ReadAllText(path);
		if (loadedFile != Filesystem::FILE_NOT_OPEN)
		{
			string currentPath = "";
			for (string line: Utils::Split(loadedFile, "\n"))
			{
				if (strncmp(line.c_str(), "ArmaPath=", 9) == 0)
					ArmaPath = line.substr(9);
				else if (strncmp(line.c_str(), "WorkshopPath=", 13) == 0)
					WorkshopPath = line.substr(13);
				else if (strncmp(line.c_str(), "WindowSizeX=", 12) == 0)
					WindowSizeX = strtol(line.substr(12).c_str(), NULL, 10);
				else if (strncmp(line.c_str(), "WindowSizeY=", 12) == 0)
					WindowSizeY = strtol(line.substr(12).c_str(), NULL, 10);
				else if (strncmp(line.c_str(), "WindowPosX=", 11) == 0)
					WindowPosX = strtol(line.substr(11).c_str(), NULL, 10);
				else if (strncmp(line.c_str(), "WindowPosY=", 11) == 0)
					WindowPosY = strtol(line.substr(11).c_str(), NULL, 10);
				else
					LOG(1, "Invalid line in config file " + path);
			}
		}
		else
			LOG(1, "Can't load settings!");
		return true;
	}

	bool Save(string path)
	{
		string outFile = "ArmaPath=" + ArmaPath + "\nWorkshopPath=" + WorkshopPath
				+ "\nWindowSizeX=" + to_string(WindowSizeX) + "\nWindowSizeY="
				+ to_string(WindowSizeY) + "\nWindowPosX=" + to_string(WindowPosX)
				+ "\nWindowPosY=" + to_string(WindowPosY);
		if (!Filesystem::WriteAllText(path, outFile))
		{
			LOG(1, "Can't write to settings file!");
			return false;
		}
		return true;
	}

	vector<Mod> LoadUserMods()
	{
		vector<Mod> response;
		return response;
	}

	bool SaveUserMods(vector<Mod> mods)
	{
		return true;
	}
}
