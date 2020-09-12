#pragma once

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
#include <type_traits>
#include <vector>

#include "exceptions/path_no_access.hpp"

#include "string_utils.hpp"

namespace StdUtils
{
    template <typename T>
    struct IsStdMap
    {
        static constexpr bool value = false;
    };

    template<typename Key, typename Value>
    struct IsStdMap<std::map<Key, Value>>
    {
        static constexpr bool value = true;
    };

    template<typename T, typename container_t = std::vector<T>>
    bool Contains(container_t const &container, T const &t)
    {
        static_assert(!IsStdMap<container_t>::value, "Use ContainsKey for map");
        return std::find(container.begin(), container.end(), t) != container.end();
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
    std::pair<int, std::string> ExecuteCommand(std::string_view const command);
    std::string FileReadAllText(std::filesystem::path const &path);
    void FileWriteAllText(std::filesystem::path const &path, std::string const &text);
    pid_t IsProcessRunning(std::string const &name, bool case_insensitive = false);
    void StartBackgroundProcess(std::string const &command, std::string_view const working_directory = "");
    std::filesystem::path GetConfigFilePath(std::filesystem::path const &config_filename);
    bool IsLibraryAvailable(char const *library_filename);
}
