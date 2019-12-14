/*
 * Utils.cpp
 *
 *  Created on: 20 Oct 2016
 *      Author: muttley
 */

#include "Utils.h"

#include <cstring>
#include <algorithm>

#include <cstdlib>
#include <cstdio>
#include <dirent.h>

#include "Filesystem.h"
#include "Logger.h"

using namespace std;

namespace Utils
{
    vector<string> Split(std::string textToSplit, std::string delimiters)
    {
        vector<string> response;
        char *text = new char[textToSplit.size() + 1];
        strcpy(text, textToSplit.c_str());
        char *strPtr = strtok(text, delimiters.c_str());
        while (strPtr != NULL)
        {
            string toAdd = strPtr;
            toAdd = Trim(toAdd);
            if (toAdd.length() > 0)
                response.push_back(strPtr);
            strPtr = strtok(NULL, delimiters.c_str());
        }
        delete[] text;
        return response;
    }

    pair<string, string> SplitFirst(string textToSplit, string delimiters)
    {
        auto position = textToSplit.find_first_of(delimiters);
        return pair<string, string>{textToSplit.substr(0, position), textToSplit.substr(position + 1)};
    }

    bool EndsWith(string textToCheck, string textToFind)
    {
        if (textToFind.size() > textToCheck.size())
            return false;
        char *textToCheckPtr = (char *)textToCheck.c_str() + textToCheck.size() - textToFind.size();
        if (strcmp(textToCheckPtr, textToFind.c_str()) == 0)
            return true;
        return false;
    }

    bool StartsWith(string textToCheck, string textToFind)
    {
        if (textToFind.size() > textToCheck.size())
            return false;
        if (strncmp(textToCheck.c_str(), textToFind.c_str(), textToFind.size()) == 0)
            return true;
        return false;
    }

    string Replace(string str, string from, string to)
    {
        size_t start_pos = 0;
        while (start_pos != std::string::npos)
        {
            start_pos = str.find(from, start_pos);
            if (start_pos == std::string::npos)
                return str;
            str.replace(start_pos, from.length(), to);
            //move start_pos forward so it doesn't replace the same string over and over
            start_pos += to.length() - from.length() + 1;
        }
        return str;
    }

    string Trim(const string &s)
    {
        return TrimRight(TrimLeft(s));
    }

    string TrimLeft(const string &s)
    {
        size_t startpos = s.find_first_not_of(" \n\r\t");
        return (startpos == std::string::npos) ? "" : s.substr(startpos);
    }

    string TrimRight(const string &s)
    {
        size_t endpos = s.find_last_not_of(" \n\r\t");
        return (endpos == std::string::npos) ? "" : s.substr(0, endpos + 1);
    }

    string RemoveLastElement(string s, bool removeSlash, int count)
    {
        if (s.length() == 0)
            return "";
        reverse(s.begin(), s.end());

        for (int i = 0; i < count; i++)
        {
            size_t slashPos = s.find("/");
            slashPos++;

            s = s.substr(slashPos);
        }
        if (!removeSlash)
            s = "/" + s;
        reverse(s.begin(), s.end());

        return s;
    }

    string ToString(bool b)
    {
        if (b)
            return "1";
        return "0";
    }

    pid_t FindProcess(string name)
    {
        #ifdef __APPLE__
        char buffer[128];
        std::string result = "";
        std::string cmd = "ps -Ac | grep " + name;
        std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
        if (!pipe) throw std::runtime_error("popen() failed!");
        while (!feof(pipe.get()))
        {
            if (fgets(buffer, 128, pipe.get()) != NULL)
                result += buffer;
        }
        if (result.length() < 1)
            return -1;
        vector<string> splits = Split(result, " ");
        if (splits.size() > 0)
            return strtol(splits[0].c_str(), NULL, 10);
        return -1;
        #else

        for (auto const &process : Filesystem::GetSubDirectories("/proc"))
        {
            auto process_name = Utils::Trim(Filesystem::ReadAllText("/proc/" + process + "/comm", true));
            if (process_name == name)
                return std::stoi(process);
        }

        return -1;
        #endif
    }

    string BashAdaptPath(string path)
    {
        return Utils::Replace(path, " ", "\\ ");
    }

    bool ContainsAddons(const std::string &path)
    {
        for (auto dir : Filesystem::GetSubDirectories(path))
        {
            std::transform(dir.begin(), dir.end(), dir.begin(), ::tolower);
            if (dir == "addons")
                return true;
        }
        return false;
    }

    bool ContainsCppFile(const std::string &path)
    {
        return Filesystem::FileExists(path + "/meta.cpp") || Filesystem::FileExists(path + "/mod.cpp");
    }
}
