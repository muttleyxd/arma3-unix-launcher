/*
 * Utils.h
 *
 *  Created on: 20 Oct 2016
 *      Author: muttley
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <vector>
#include <string>

class Utils
{
	public:
		Utils();
		~Utils();

		std::vector<std::string> Split(std::string textToSplit, std::string delimiters);
		bool StartsWith(std::string textToCheck, std::string textToFind);
		std::string Replace(std::string str, std::string from, std::string to);
		std::string Trim(const std::string& s);
		std::string TrimLeft(const std::string& s);
		std::string TrimRight(const std::string& s);
		std::string RemoveLastElement(std::string s, bool removeSlash);
};

#endif /* UTILS_H_ */
