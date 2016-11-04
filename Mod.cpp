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
	ParseCPP(metaPath, modPath);

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
void Mod::ParseCPP(string meta, string mod)
{
    if (meta != "")
    {
        string file = ParseString(Filesystem::ReadAllText(meta));
        //cout << "Meta.cpp: "<< file << endl;
        vector<string> instructions = Utils::Split(file, ";");
        for (string s: instructions)
        {
            if (Utils::StartsWith(s, "name"))
            {
                //name="hello" -> hello
                this->Name = s.substr(6, s.size() - 7);
                this->DirName = s.substr(6, s.size() - 7);
            }
            else if (Utils::StartsWith(s, "publishedid"))
                this->PublishedId = s.substr(12);
        }

    }
	if (mod != "")
	{
		string file = ParseString(Filesystem::ReadAllText(mod));
		vector<string> instructions = Utils::Split(file, ";");
		for (string s: instructions)
		{
			if (Utils::StartsWith(s, "name"))
				this->Name = s.substr(6, s.size() - 7);
			else if (Utils::StartsWith(s, "picture"))
				this->Picture = s.substr(9, s.size() - 10);
			else if (Utils::StartsWith(s, "logoSmall"))
				this->LogoSmall = s.substr(11, s.size() - 12);
			else if (Utils::StartsWith(s, "logo"))
				this->Logo = s.substr(6, s.size() - 7);
			else if (Utils::StartsWith(s, "logoOver"))
				this->LogoOver = s.substr(10, s.size() - 11);
			else if (Utils::StartsWith(s, "action"))
				this->Action = s.substr(8, s.size() - 9);
			else if (Utils::StartsWith(s, "actionName"))
				this->ActionName = s.substr(12, s.size() - 13);
			else if (Utils::StartsWith(s, "tooltipOwned"))
				this->TooltipOwned = s.substr(14, s.size() - 15);
			else if (Utils::StartsWith(s, "overview"))
				this->Overview = s.substr(10, s.size() - 11);
			else if (Utils::StartsWith(s, "description"))
				this->Description = s.substr(13, s.size() - 14);
			else if (Utils::StartsWith(s, "overviewPicture"))
				this->OverviewPicture = s.substr(17, s.size() - 18);
			else if (Utils::StartsWith(s, "overviewText"))
				this->OverviewText = s.substr(14, s.size() - 15);
			else if (Utils::StartsWith(s, "author"))
				this->Author = s.substr(8, s.size() - 9);
			else if (Utils::StartsWith(s, "dlcColor"))
			{
				//dlcColor[]={1.2,5.8,3.23,6.44}
				string dlcColor = s.substr(12, dlcColor.size() - 13);
				//1.2,5.8,3.23,6.44
				vector<string> splits = Utils::Split(dlcColor,",");
				if (splits.size() != 4)
					LOG(1, "Could not read dlcColor from mod.cpp");
				else
				{
					DlcColor.r = atof(splits[0].c_str());
					DlcColor.g = atof(splits[1].c_str());
					DlcColor.b = atof(splits[2].c_str());
					DlcColor.a = atof(splits[3].c_str());
				}

			}
			/* there is no stabilized syntax upon this
			 * some write hideName=0 or hideName=1
			 * but also I've seen hideName="false"
			 * */
			else if (Utils::StartsWith(s, "hideName"))
			{
				string value = s.substr(9);
				for (char c: value)
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
			else if (Utils::StartsWith(s, "hidePicture"))
			{
				string value = s.substr(12);
				for (char c: value)
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
