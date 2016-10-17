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

using namespace std;

Settings::Settings()
{
	ArmaPath = "";
	WorkshopPath = "";

}

Settings::~Settings()
{
}

bool Settings::Load(string path)
{
	ifstream configFile;
	configFile.open(path);
	string loadedFile;
	if (configFile.is_open())
	{
		cout << "File " << path << " successfully opened...\n";
		getline(configFile, loadedFile, '\0');
		istringstream iss(loadedFile);
		string currentPath = "";
		int counter = 0;
		for (string line; getline(iss, line); counter++)
		{
			if (strncmp(line.c_str(), "ArmaPath=", 9) == 0)
				ArmaPath = line.substr(9);
			else if (strncmp(line.c_str(), "WorkshopPath=", 13) == 0)
				WorkshopPath = line.substr(13);
			else
				cout << "Invalid line in config file " << path << endl;
		}
	}
	else
	{
		cout << "Can't open " << path << "\nCan't load settings!\n\n";
	}
	return true;
}
