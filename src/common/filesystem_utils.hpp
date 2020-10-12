#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace FilesystemUtils
{
    bool CreateDirectories(std::filesystem::path const &path);
    std::filesystem::path CurrentPath();
    void CurrentPath(std::filesystem::path const &path);
    bool Exists(std::filesystem::path const &path);
    bool IsDirectory(std::filesystem::path const &path);
    std::vector<std::string> Ls(std::filesystem::path const &path, bool set_lowercase = false);
    std::filesystem::path RealPath(std::filesystem::path const &path);
    uintmax_t RemoveAll(std::filesystem::path const &path);
    std::filesystem::path TempDirectoryPath();
}
