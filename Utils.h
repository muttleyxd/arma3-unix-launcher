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

#include <sys/types.h>
#include <unistd.h>

namespace Utils
{
    std::vector<std::string> Split(std::string textToSplit, std::string delimiters);
    bool EndsWith(std::string textToCheck, std::string textToFind);
    bool StartsWith(std::string textToCheck, std::string textToFind);
    std::string Replace(std::string str, std::string from, std::string to);
    std::string Trim(const std::string &s);
    std::string TrimLeft(const std::string &s);
    std::string TrimRight(const std::string &s);
    std::string RemoveLastElement(std::string s, bool removeSlash, int count = 1);
    std::string ToString(bool b);
    pid_t FindProcess(std::string name);
    std::string BashAdaptPath(std::string path);
    bool ContainsAddons(const std::string &path);
};

#endif /* UTILS_H_ */
