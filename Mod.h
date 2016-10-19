/*
 * Mod.h
 *
 *  Created on: 18 Oct 2016
 *      Author: muttley
 */

#ifndef MOD_H_
#define MOD_H_

#include <string>

#include "Vec4.h"

class Mod
{
	public:
		Mod(std::string path);
		~Mod();

		std::string Path, Name, Picture, LogoSmall, Logo, LogoOver, Action,
				ActionName, TooltipOwned, Overview, Description, OverviewPicture,
				OverviewText, Author;
		Vec4 DlcColor;
		bool HideName, HidePicture;
		int64_t PublishedId;

	private:
		void ParseCPP(std::string mod, std::string meta);
		std::string ParseString(std::string input);
};

#endif /* MOD_H_ */
