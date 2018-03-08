#ifndef __SETUP_HPP
#define __SETUP_HPP

#include <string>
#include <vector>

#include <dirent.h>

std::string RemoveLastElement(std::string s, bool removeSlash, int count)
{
    if (s.length() == 0)
        return "";
    std::reverse(s.begin(), s.end());

    for (int i = 0; i < count; i++)
    {
        size_t slashPos = s.find("/");
        slashPos++;

        s = s.substr(slashPos);
    }
    if (!removeSlash)
        s = "/" + s;
    std::reverse(s.begin(), s.end());

    return s;
}

std::string GetWorkDir()
{
    std::string ret = "";
    char buf[1024];
    ssize_t result = readlink("/proc/self/exe", buf, sizeof(buf));
    if (result != 0)
    {
        buf[result] = 0;
        ret = RemoveLastElement(buf, true, 1);
    }
    return ret;
}

std::vector<std::string> Ls(std::string path)
{
    std::vector<std::string> ret;
    DIR *dir = opendir(path.c_str());
    if (!dir)
        return ret;

    dirent *dp;
    while ((dp = readdir(dir)))
        ret.push_back(dp->d_name);
    closedir(dir);
    std::sort(ret.begin(), ret.end());
    return ret;
}

#endif
