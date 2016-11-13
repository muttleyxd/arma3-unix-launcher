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

#include "Logger.h"

using namespace std;

namespace Utils
{
    vector<string> Split(std::string textToSplit, std::string delimiters)
    {
        vector<string> response;
        char* text = new char[textToSplit.size() + 1];
        strcpy(text, textToSplit.c_str());
        char* strPtr = strtok(text, delimiters.c_str());
        while (strPtr != NULL)
        {
            string toAdd = strPtr;
            toAdd = Trim(toAdd);
            if (toAdd.length() > 0)
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

    string Replace(string str, string from, string to)
    {
        size_t start_pos = 0;
        while (start_pos != std::string::npos)
        {
            start_pos = str.find(from, start_pos);
            if(start_pos == std::string::npos)
                return str;
            str.replace(start_pos, from.length(), to);
            //move start_pos forward so it doesn't replace the same string over and over
            start_pos += to.length() - from.length() + 1;
        }
        return str;
    }

    string Trim(const string& s)
    {
        return TrimRight(TrimLeft(s));
    }

    string TrimLeft(const string& s)
    {
        size_t startpos = s.find_first_not_of(" \n\r\t");
        return (startpos == std::string::npos) ? "" : s.substr(startpos);
    }

    string TrimRight(const string& s)
    {
        size_t endpos = s.find_last_not_of(" \n\r\t");
        return (endpos == std::string::npos) ? "" : s.substr(0, endpos+1);
    }

    string RemoveLastElement(string s, bool removeSlash, int count)
    {
        if (s.length() == 0)
            return "";
        reverse(s.begin(), s.end());

        for (int i = 0; i < count; i++)
        {
			size_t slashPos = s.find("/");

			if (removeSlash || i != count - 1)
				slashPos++;

			s = s.substr(slashPos);
        }
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
        DIR* dir;
        struct dirent* ent;
        char* endptr;
        char buf[512];

        if (!(dir = opendir("/proc")))
            return -1;

        while((ent = readdir(dir)) != NULL)
        {
            long lpid = strtol(ent->d_name, &endptr, 10);
            if (*endptr != '\0')
                continue;

            snprintf(buf, sizeof(buf), "/proc/%ld/cmdline", lpid);
            FILE* fp = fopen(buf, "r");

            if (fp)
            {
                if (fgets(buf, sizeof(buf), fp) != NULL)
                {
                    char* first = strtok(buf, " ");
                    //LOG(1, first);
                    if (!strcmp(first, name.c_str()))
                    {
                        fclose(fp);
                        closedir(dir);
                        return (pid_t)lpid;
                    }
                }
                fclose(fp);
            }

        }

        closedir(dir);
        return -1;
    }

    string BashAdaptPath(string path)
    {
        return Utils::Replace(path, " ", "\\ ");
    }
}
