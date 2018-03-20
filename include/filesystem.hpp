#ifndef __FILESYSTEM_HPP
#define __FILESYSTEM_HPP

#include <string>
#include <vector>

namespace Filesystem
{
    int FileCreate(const std::string &path, const std::string &content = "");
    int FileDelete(const std::string &path);
    int FileExists(const std::string &path);

    std::string FileReadAllText(const std::string &path);

    int DirectoryCreate(const std::string &path);
    int DirectoryDelete(const std::string &path, bool recursive = false);
    int DirectoryExists(const std::string &path);

    // This is not recursive
    std::vector<std::string> GetSubdirectories(const std::string &path);

    int SymlinkCreate(const std::string &source, const std::string &target);
    std::string SymlinkGetTarget(const std::string &source);
}

#endif
