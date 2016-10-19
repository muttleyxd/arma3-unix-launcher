/*
 * Utils.cpp
 *
 *  Created on: 20 Oct 2016
 *      Author: muttley
 */

#include "Utils.h"

#include <cstring>

using namespace std;

Utils::Utils()
{
}

Utils::~Utils()
{
}

vector<string> Utils::Split(std::string textToSplit, std::string delimiters)
{
	vector<string> response;
	char* text = new char[textToSplit.size() + 1];
	strcpy(text, textToSplit.c_str());
	char* strPtr = strtok(text, delimiters.c_str());
	while (strPtr != NULL)
	{
		response.push_back(strPtr);
		strPtr = strtok(NULL, delimiters.c_str());
	}
	return response;
}

bool StartsWith(string textToCheck, string textToFind)
{
	if (textToFind.size() > textToCheck.size())
		return false;
	if (strncmp(textToCheck.c_str(), textToFind.c_str(), textToFind.size()) == 0)
		return true;
	return false;
}
