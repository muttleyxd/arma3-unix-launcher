/*
 * Mod.cpp
 *
 *  Created on: 18 Oct 2016
 *      Author: muttley
 */

#include "Mod.h"

#include <regex>
#include <cstdlib>
#include <iostream>

#include "Utils.h"
#include "Filesystem.h"
#include "Settings.h"
#include "Logger.h"

using namespace std;

// workshopId = -1 -> mod is not from workshop
// workshopId > 1 -> mod from workshop, workshop
Mod::Mod(string path, string workshopId)
{
    Path = path;
    Name = DirName = Picture = LogoSmall = Logo = LogoOver = Action = TooltipOwned = Overview = "";
    DlcColor = {-1, -1, -1, -1};
    HideName = HidePicture = false;
    PublishedId = "-1";
    IsRepresentedBySymlink = false;

    Enabled = false;

    WorkshopId = workshopId;

    string metaPath = path + "/meta.cpp", modPath = path + "/mod.cpp";
    if (!Filesystem::FileExists(metaPath))
        metaPath = "";
    if (!Filesystem::FileExists(modPath))
        modPath = "";
    ParseCPP(metaPath, modPath, path, workshopId);

    //Mod is added manually by user and din't contain any data in meta.cpp or mod.cpp
    //quick & dirty -> Name = DirectoryName
    if (Name == "" || DirName == "")
    {
        int withoutLastElement = Utils::RemoveLastElement(path, false).size();
        if (Name == "")
            Name = path.substr(withoutLastElement);
        if (DirName == "")
            DirName = path.substr(withoutLastElement);
    }
}

Mod::~Mod()
{
}

string Mod::ParseString(string input)
{
    string response = input;

    regex removeComments("(/\\*.*?\\*/|//[^\\r\\n]*$)");
    regex removeWhitespaces("\\s+(?=([^\"]*\"[^\"]*\")*[^\"]*$)");

    response = regex_replace(response, removeComments, "");
    response = regex_replace(response, removeWhitespaces, "");

    return response;
}

//As anyone would expect - documentation on this is trash
void Mod::ParseCPP(string meta, string mod, string path, string workshopId)
{
    if (meta != "")
    {
        string file = ParseString(Filesystem::ReadAllText(meta));
        //cout << "Meta.cpp: "<< file << endl;
        vector<string> instructions = Utils::Split(file, ";");
        for (string s : instructions)
        {
            if (Utils::StartsWith(s, "name"))
            {
                //name="hello" -> hello
                this->Name = s.substr(6, s.size() - 7);
            }
            else if (Utils::StartsWith(s, "publishedid"))
                this->PublishedId = s.substr(12);
        }

    }
    if (mod != "")
    {
        string file = ParseString(Filesystem::ReadAllText(mod));
        vector<string> instructions = Utils::Split(file, ";");
        for (string s : instructions)
        {
            auto keyValue = Utils::SplitFirst(s, "=");
            auto const key = Utils::Replace(Utils::Trim(keyValue.first), "\"", "");
            auto const value = Utils::Replace(Utils::Trim(keyValue.second), "\"", "");
            if (key == "name")
                this->Name = value;
            else if (key ==  "picture")
                this->Picture = value;
            else if (key ==  "logoSmall")
                this->LogoSmall = value;
            else if (key ==  "logo")
                this->Logo = value;
            else if (key ==  "logoOver")
                this->LogoOver = value;
            else if (key ==  "action")
                this->Action = value;
            else if (key ==  "actionName")
                this->ActionName = value;
            else if (key ==  "tooltipOwned")
                this->TooltipOwned = value;
            else if (key ==  "overview")
                this->Overview = value;
            else if (key ==  "description")
                this->Description = value;
            else if (key ==  "overviewPicture")
                this->OverviewPicture = value;
            else if (key ==  "overviewText")
                this->OverviewText = value;
            else if (key ==  "author")
                this->Author = value;
            /* there is no stabilized syntax upon this
             * some write hideName=0 or hideName=1
             * but also I've seen hideName="false"
             * */
            else if (key == "hideName")
            {
                for (char c : value)
                {
                    if (c == '0' || c == 'f')
                    {
                        this->HideName = false;
                        break;
                    }
                    else if (c == '1' || c == 't')
                    {
                        this->HideName = true;
                        break;
                    }
                }
            }
            else if (key == "hidePicture")
            {
                for (char c : value)
                {
                    if (c == '0' || c == 'f')
                    {
                        this->HidePicture = false;
                        break;
                    }
                    else if (c == '1' || c == 't')
                    {
                        this->HidePicture = true;
                        break;
                    }
                }
            }
        }
    }
    if (workshopId == "-1")
    {
        this->DirName = Utils::Replace(path, Settings::ArmaPath + "/", "");
    }
    if (this->DirName.empty())
    {
        this->DirName = this->Name;
    }
    this->DirName = Utils::Replace(this->DirName, "/", "_");
}

string Mod::ToString()
{
    string response = "";

    response += "Path: " + Path + "\n";
    response += "Name: " + Name + "\n";
    response += "Picture: " + Picture + "\n";
    response += "LogoSmall: " + LogoSmall + "\n";
    response += "Logo: " + Logo + "\n";
    response += "LogoOver: " + LogoOver + "\n";
    response += "Action: " + Action + "\n";
    response += "ActionName: " + ActionName + "\n";
    response += "TooltipOwned: " + TooltipOwned + "\n";
    response += "Overview: " + Overview + "\n";
    response += "Description: " + Description + "\n";
    response += "OverviewPicture: " + OverviewPicture + "\n";
    response += "OverviewText: " + OverviewText + "\n";
    response += "Author: " + Author + "\n";

    response += "DlcColor r:" + to_string(DlcColor.r) + " g:" + to_string(DlcColor.g)
                + " b:" + to_string(DlcColor.b) + " a:" + to_string(DlcColor.a) + "\n";

    response += "HideName: " + to_string(HideName) + "\n";
    response += "HidePicture: " + to_string(HidePicture) + "\n";

    response += "PublishedId: " + PublishedId + "\n";

    return response;
}
