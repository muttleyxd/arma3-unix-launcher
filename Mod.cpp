/*
 * Mod.cpp
 *
 *  Created on: 18 Oct 2016
 *      Author: muttley
 */

#include "Mod.h"

#include <regex>

#include "Utils.h"

using namespace std;

Mod::Mod(string path)
{
	Path = path;
	Name = Picture = LogoSmall = Logo = LogoOver = Action = TooltipOwned = Overview = "";
	DlcColor = {-1, -1, -1, -1};
	HideName = HidePicture = false;
	PublishedId = -1;
}

Mod::~Mod()
{
}

string Mod::ParseString(string input)
{
	string response = input;
	input = regex_replace(input, removeSinglelineComments, "");
	input = regex_replace(input, removeMultilineComments, "");
	input = regex_replace(input, removeWhitespaces, "");
	return response;
}



void Mod::ParseCPP(string mod, string meta)
{
	regex removeSinglelineComments("//(.*?)\\r?\\n");
	regex removeMultilineComments("/\\*(?:.|[\\n\\r])*?\\*/");
	regex removeWhitespaces("\\s+(?=([^\"]*\"[^\"]*\")*[^\"]*$)");

	Filesystem fs;
	Utils us;

	if (mod != "")
	{
		string file = ParseString(fs.ReadAllText(mod));
		vector<string> instructions = us.Split(file, ";");
		for (string s: instructions)
		{
			if (us.StartsWith(s, "name"))
			{

			}
			else if (us.StartsWith(s, "picture"))
			{

			}
			else if (us.StartsWith(s, "name"))
			{

			}
			else if (us.StartsWith(s, "name"))
			{

			}
			else if (us.StartsWith(s, "name"))
			{

			}
			else if (us.StartsWith(s, "name"))
			{

			}
			else if (us.StartsWith(s, "name"))
			{

			}
			else if (us.StartsWith(s, "name"))
			{

			}
			else if (us.StartsWith(s, "name"))
			{

			}
			else if (us.StartsWith(s, "name"))
			{

			}
		}
	}
	if (meta != "")
	{

	}
}
