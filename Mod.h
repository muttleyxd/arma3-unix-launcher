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
        Mod(std::string path, std::string workshopId);
        ~Mod();

        std::string Path, Name, DirName, Picture, LogoSmall, Logo, LogoOver, Action,
            ActionName, TooltipOwned, Overview, Description, OverviewPicture,
            OverviewText, Author;
        Vec4 DlcColor;
        bool HideName, HidePicture;
        std::string PublishedId;
        std::string WorkshopId;

        std::string ToString();

        bool IsRepresentedBySymlink;

        bool Enabled;

    private:
        void ParseCPP(std::string meta, std::string mod);
        std::string ParseString(std::string input);
};

#endif /* MOD_H_ */
