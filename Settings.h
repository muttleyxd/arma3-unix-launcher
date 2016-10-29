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

	bool Load(std::string path);
	bool Save(std::string path);

	std::vector<Mod> LoadUserMods();
	bool SaveUserMods(std::vector<Mod> mods);
};

#endif /* SETTINGS_H_ */
