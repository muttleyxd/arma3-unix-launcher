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
#include <vector>

#include "exceptions/path_no_access.hpp"

#include "string_utils.hpp"

namespace StdUtils
{
    bool CreateFile(std::filesystem::path const &path);
    std::pair<int, std::string> ExecuteCommand(std::string_view const command);
    std::string FileReadAllText(std::filesystem::path const &path);
    void FileWriteAllText(std::filesystem::path const &path, std::string const &text);
    pid_t IsProcessRunning(std::string const &name, bool case_insensitive = false);
    void StartBackgroundProcess(std::string const &command, std::string_view const working_directory = "");
    std::filesystem::path GetConfigFilePath(std::filesystem::path const &config_filename, std::string_view const appname);
    bool IsLibraryAvailable(char const *library_filename);
}
