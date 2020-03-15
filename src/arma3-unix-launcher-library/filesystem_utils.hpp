#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace FilesystemUtils
{
    bool CreateDirectories(std::filesystem::path const &path);
    bool Exists(std::filesystem::path const &path);
    bool IsDirectory(std::filesystem::path const &path);
    std::vector<std::string> Ls(std::filesystem::path const &path, bool set_lowercase = false);
}
