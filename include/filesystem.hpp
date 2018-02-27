#ifndef __FILESYSTEM_HPP
#define __FILESYSTEM_HPP

#include <string>
#include <vector>

namespace Filesystem
{
    int FileCreate(std::string path, std::string content = "");
    int FileDelete(std::string path);
    int FileExists(std::string path);

    std::string FileReadAllText(std::string path);

    int DirectoryCreate(std::string path);
    int DirectoryDelete(std::string path, bool recursive = false);
    int DirectoryExists(std::string path);

    // This is not recursive
    std::vector<std::string> GetSubdirectories(std::string path);

    int CreateSymlink(std::string source, std::string target);
    std::string GetSymlinkTarget(std::string source);
}

#endif
