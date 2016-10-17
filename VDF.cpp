/*
 * VDF.cpp
 *
 *  Created on: 9 Oct 2016
 *      Author: muttley
 */

#include "VDF.h"

#include <sstream>
#include <cctype>
#include <algorithm>
#include <iostream>
#include <cstring>

using namespace std;

inline string trim(const string &s)
{
   auto wsfront=find_if_not(s.begin(),s.end(),[](int c){return isspace(c);});
   auto wsback=find_if_not(s.rbegin(),s.rend(),[](int c){return isspace(c);}).base();
   return (wsback<=wsfront ? string() : string(wsfront,wsback));
}

vector<string> split(string s, string delimiters)
{
	vector<string> response;
	char* charPointer = strtok((char*)s.c_str(), delimiters.c_str());
	while (charPointer != NULL)
	{
		string toAdd = charPointer;
		toAdd=trim(toAdd);
		if (toAdd.length() > 0)
			response.push_back(toAdd);
		charPointer = strtok(NULL, delimiters.c_str());
	}
	return response;
}

string removeLastElement(string s, bool removeSlash)
{
	if (s.length() == 0)
		return "";
	reverse(s.begin(), s.end());

	size_t slashPos = s.find("/");

	if (removeSlash)
		slashPos++;

	s = s.substr(slashPos);
	reverse(s.begin(), s.end());

	return s;
}

VDF::VDF(string text)
{
	istringstream iss(text);
	string currentPath = "";
	int counter = 0;
	for (string line; getline(iss, line); counter++)
	{
		vector<string> splits = split(line, "\"");
		if (splits.size() == 0)
			continue;
		string newPath;
		switch (splits[0][0])
		{
			case '{':
				currentPath += '/';
				break;
			case '}':
				currentPath = removeLastElement(currentPath, true);
				break;
			default:
				currentPath = removeLastElement(currentPath, false) + splits[0];
				if (splits.size() > 1)
					Keys.push_back(VDFKey(currentPath, splits[1]));
				else
					Keys.push_back(VDFKey(currentPath, ""));
				break;
		}
	}
	/*for (VDFKey v: Keys)
	{
			cout << "Key: " << v.Path;
			if (v.Value != "")
				cout << " Value: " << v.Value;
			cout << endl;
	}*/
}

string VDF::GetValue(string KeyName)
{
	for (VDFKey v: Keys)
		if (v.Path == KeyName)
			return v.Value;
	return KEY_NOT_FOUND;
}
