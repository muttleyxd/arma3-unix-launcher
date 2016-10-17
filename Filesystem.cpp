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

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#include "VDF.h"

using namespace std;

//working algorithm
Filesystem::Filesystem()
{

}

Filesystem::~Filesystem()
{

}

std::vector<std::string> Filesystem::GetSteamLibraries()
{
	vector<string> response;
	string steamConfigFile;
	ifstream configFile;
	configFile.open(HomeDirectory + STEAM_CONFIG_FILE, ios::in);
	if (configFile.is_open())
	{
		cout << "File " << HomeDirectory + STEAM_CONFIG_FILE << " successfully opened...\nReading libraries list...\n";
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
	{
		cout << "Can't open " << HomeDirectory + STEAM_CONFIG_FILE << "\nCritical error\n\n";
	}
	return response;
}

std::string Filesystem::GetDirectory(DirectoryToFind dtf)
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
	return "Directory not found";
}

bool Filesystem::FileExists(string path)
{
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}


void Filesystem::WriteAllText(string path, string value)
{
	cout << "Writing to file " << path << endl;
	ofstream outFile(path);
	outFile << value;
	outFile.close();
}

string Filesystem::ReadAllText(string path)
{
	cout << "Reading file " << path << endl;
	string response;
	ifstream inFile;
	inFile.open(path, ios::in);
	getline(inFile, response, '\0');
	inFile.close();
	return response;
}
