#ifndef __FILESYSTEM_HPP
#define __FILESYSTEM_HPP

#include <string>
#include <vector>

namespace Filesystem
{
    int FileCreate(const std::string &path, const std::string &content = "") noexcept;
    int FileDelete(const std::string &path) noexcept;
    bool FileExists(const std::string &path) noexcept;

    std::string FileReadAllText(const std::string &path);

    int DirectoryCreate(const std::string &path) noexcept;
    int DirectoryDelete(const std::string &path, bool recursive = false) noexcept;
    bool DirectoryExists(const std::string &path) noexcept;

    // This is not recursive
    std::vector<std::string> Ls(const std::string &path, bool set_to_lowercase = false);

    int SymlinkCreate(const std::string &source, const std::string &target) noexcept;
    bool SymlinkExists(const std::string &path) noexcept;
    std::string SymlinkGetTarget(const std::string &source);
}

#endif
