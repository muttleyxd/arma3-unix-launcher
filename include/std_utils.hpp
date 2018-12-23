#ifndef STD_UTILS_HPP_
#define STD_UTILS_HPP_

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include "exceptions/path_no_access.hpp"

#include "string_utils.hpp"

namespace StdUtils
{
    template<typename T, typename container = std::vector<T>>
    bool Contains(container const &cnt, T const &t)
    {
        return std::find(cnt.begin(), cnt.end(), t) != cnt.end();
    }

    template<typename T, typename X>
    bool ContainsKey(std::map<T, X> const &map, T const &key)
    {
        return map.find(key) != map.end();
    }

    template<typename T>
    bool ContainsKey(std::map<std::string, T> const &map, char const *const key)
    {
        return map.find(std::string(key)) != map.end();
    }

    bool CreateFile(std::filesystem::path const &path);
    std::vector<std::string> Ls(std::filesystem::path const &path, bool set_lowercase = false);
    std::string FileReadAllText(std::filesystem::path const &path);
    void FileWriteAllText(std::filesystem::path const &path, std::string const &text);
}

#endif
