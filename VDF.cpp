/*
 * VDF.cpp
 *
 *  Created on: 9 Oct 2016
 *      Author: muttley
 */

#include "VDF.h"

#include <sstream>
#include <iostream>

#include "Utils.h"

using namespace std;

VDF::VDF(string text)
{
	Utils utils;
	istringstream iss(text);
	string currentPath = "";
	int counter = 0;
	cout << "VDF constructor\n";
	for (string line; getline(iss, line); counter++)
	{
		vector<string> splits = utils.Split(utils.Trim(line), "\"");
		if (splits.size() == 0)
			continue;
		string newPath;
		switch (splits[0][0])
		{
			case '{':
				currentPath += '/';
				break;
			case '}':
				currentPath = utils.RemoveLastElement(currentPath, true);
				break;
			default:
				currentPath = utils.RemoveLastElement(currentPath, false) + splits[0];
				if (splits.size() > 1)
					Keys.push_back(VDFKey(currentPath, splits[1]));
				else
					Keys.push_back(VDFKey(currentPath, ""));
				break;
		}
	}
	/*for (VDFKey f: Keys)
		cout << "Key: " << f.Path << (f.Value != "" ? " Value: " : "") << (f.Value != "" ? f.Value : "") << endl;*/
}

string VDF::GetValue(string KeyName)
{
	for (VDFKey v: Keys)
		if (v.Path == KeyName)
			return v.Value;
	return KEY_NOT_FOUND;
}
