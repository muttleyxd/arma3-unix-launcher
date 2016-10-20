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

bool Filesystem::DirectoryExists(string path)
{
	struct stat buffer;
	int errorCode = stat(path.c_str(), &buffer);
	if (errorCode == -1)
	{
		if (errno == ENOENT)
			return false;
		cout << "Stat Error in DirectoryExists(" << path <<");\n";
		return false;
	}
	else
	{
		if (S_ISDIR(buffer.st_mode))
			return true;
		return false;
	}
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

vector<Mod> Filesystem::FindMods(string path)
{
	vector<Mod> response;

	for (string s: GetSubDirectories(path))
	{
		if (DirectoryExists(path + "/" + s + "/Addons")
		 || DirectoryExists(path + "/" + s + "/addons"))
			response.push_back(Mod(path + "/" + s, s));
	}
	return response;
}

void Filesystem::CheckFileStructure(string armaDir, string workshopDir, vector<Mod> modList)
{
	Utils utils;
	string armaDirWorkshopPath = armaDir + "/!workshop";
	if (!DirectoryExists(armaDirWorkshopPath))
	{
		if (!CreateDirectory(armaDirWorkshopPath))
			return;
	}

	if (!DirectoryExists(armaDirWorkshopPath + "/!DO_NOT_CHANGE_FILES_IN_THESE_FOLDERS"))
	{
		if (!CreateDirectory(armaDirWorkshopPath + "/!DO_NOT_CHANGE_FILES_IN_THESE_FOLDERS"))
			return;
	}

	vector<string> ModDirs = GetSubDirectories(armaDirWorkshopPath);
	/*for (int i = 0; i < ModDirs.size(); i++)
	{
		if (ModDirs[i] == "!DO_NOT_CHANGE_FILES_IN_THESE_FOLDERS")
		{
			ModDirs.erase(ModDirs.begin() + i);
			break;
		}
	}*/

	for (string s: ModDirs)
	{
		struct stat statinfo;
		if (lstat((armaDirWorkshopPath + "/" + s).c_str(), &statinfo) < 0)
		{
			cout << "Can't open directory/symlink " << armaDirWorkshopPath + "/" + s << endl;
			continue;
		}
		if (S_ISLNK (statinfo.st_mode))
		{
			char* buffer = new char[PATH_MAX + 1];
			size_t hello = readlink((armaDirWorkshopPath + "/" + s).c_str(), buffer, PATH_MAX);
			buffer[hello] = '\0';
			string target = buffer;
			delete[] buffer;
			cout << "!workshop/" << s << "points to: " << target << endl;
			int targetLength = target.length();
			target = utils.Replace(target, workshopDir + "/", "");

			//outside workshop dir
			if (targetLength == target.length())
			{
				for (int i = 0; i < modList.size(); i++)
				{
					if (modList[i].Path == target)
					{
						modList[i].IsRepresentedBySymlink = true;
						break;
					}
				}
			}
			else
			{
				int64_t newWorkshopId = strtoll(target.c_str(), NULL, 10);
				for (int i = 0; i < modList.size(); i++)
				{
					if (modList[i].WorkshopId == newWorkshopId)
					{
						modList[i].IsRepresentedBySymlink = true;
						break;
					}
				}
			}
		}
	}

	for (Mod m: modList)
	{
		if (!m.IsRepresentedBySymlink)
		{
			string linkName = armaDirWorkshopPath + "/@" + m.Name;
			if (!DirectoryExists(linkName) || !FileExists(linkName))
			{
				int result = symlink(m.Path.c_str(), linkName.c_str());
				if (result != 0)
					cout << "Symlink creation failed: " << m.Path << "->" << linkName << endl;
				else
					cout << "Symlink creation success: " << m.Path << "->" << linkName << endl;
			}
			else
				cout << "Dir/file " << linkName << " already exists\n";
		}
	}
}

vector<string> Filesystem::GetSubDirectories(string path)
{
	vector<string> response;
	struct dirent* directoryEntry;
	cout << "Opening directory " << path << "...";
	DIR* pathDir = opendir(path.c_str());

	if (pathDir == NULL)
	{
		cout << " failed!\n";
		return response;
	}
	cout << " success\n";

	while ((directoryEntry = readdir(pathDir)) != NULL)
	{
		struct stat st;

		if ((strcmp(directoryEntry->d_name, ".") == 0) || (strcmp(directoryEntry->d_name, "..") == 0))
			continue;

		if (fstatat(dirfd(pathDir), directoryEntry->d_name, &st, 0) < 0)
		{
			cout << "Directory error " << directoryEntry->d_name << "\n";
			continue;
		}

		if (S_ISDIR(st.st_mode))
			response.push_back(directoryEntry->d_name);
	}

	closedir(pathDir);

	return response;
}

bool Filesystem::CreateDirectory(string path)
{
	int status = mkdir(path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH); //d rwx r-x r-x
	if (status != 0)
	{
		cout << "Can't create directory " << path << endl;
		return false;
	}
	cout << "Successfully created directory " << path << endl;
	return true;
}
