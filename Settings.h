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

class Settings
{
	public:
		Settings();
		~Settings();

		std::string ArmaPath;
		std::string WorkshopPath;

		bool Load(std::string path);
		bool Save(std::string path);
};

#endif /* SETTINGS_H_ */
