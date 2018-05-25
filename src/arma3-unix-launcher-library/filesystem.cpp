#include "filesystem.hpp"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstring>
#include <memory>

namespace Filesystem
{
    int DirectoryCreate(const std::string &path) noexcept
    {
        return mkdir(path.c_str(), 0755);
    }

    int DirectoryDelete(const std::string &path, bool recursive) noexcept
    {
        if (!recursive)
            return rmdir(path.c_str());
        // Should've been implemented properly
        return system(("rm -rf " + path).c_str());
    }

    bool DirectoryExists(const std::string &path) noexcept
    {
        struct stat st;
        int result = stat(path.c_str(), &st);

        /*
         * if stat reports error - pass it further
         * if is not directory then return exists but it's not a directory
         * else return it exists
         */
        if (result != 0)
            return false;
        else if (!S_ISDIR(st.st_mode))
        {
            errno = EEXIST;
            return false;
        }
        return true;
    }

    int FileCreate(const std::string &path, const std::string &content) noexcept
    {
        int fd = creat(path.c_str(), 0644);
        if (fd == -1)
            return -1;
        int result = 0;
        if (content != "")
            result = write(fd, content.c_str(), content.size());

        close(fd);
        if (result != 0)
            return result;
        return 0;
    }

    int FileDelete(const std::string &path) noexcept
    {
        return unlink(path.c_str());
    }

    bool FileExists(const std::string &path) noexcept
    {
        struct stat st;
        int result = stat(path.c_str(), &st);

        // As in DirectoryExists
        if (result != 0)
            return false;
        else if (S_ISDIR(st.st_mode))
        {
            errno = EEXIST;
            return false;
        }
        return true;
    }

    std::string FileReadAllText(const std::string &path)
    {
        int fd = open(path.c_str(), 0);
        if (fd == -1)
            return "";
        off_t file_size = lseek(fd, 0, SEEK_END);
        lseek(fd, 0, SEEK_SET);

        std::unique_ptr<char[]> buffer = std::make_unique<char[]>(file_size);
        read(fd, buffer.get(), file_size);
        return buffer.get();
    }

    /*
     * GetSubdirectories - returns directories found in 'path'
     * This function is NOT recursive
     *
     * This function throws exceptions
     */
    std::vector<std::string> GetSubdirectories(const std::string &path)
    {
        std::vector<std::string> ret;

        DIR *dirp = opendir(path.c_str());
        if (dirp == NULL)
            throw std::invalid_argument("Error: " + std::to_string(errno) + " " + strerror(errno));

        struct dirent *dp;
        while (true)
        {
            dp = readdir(dirp);
            if (!dp)
                break;
            std::string name = dp->d_name;
            if (name != "." && name != "..")
                ret.push_back(dp->d_name);
        }

        closedir(dirp);
        return ret;
    }

    int SymlinkCreate(const std::string &source, const std::string &target) noexcept
    {
        return symlink(target.c_str(), source.c_str());
    }

    std::string SymlinkGetTarget(const std::string &source)
    {
        struct stat st;
        if (lstat(source.c_str(), &st) < 0)
            throw std::invalid_argument("Error: " + std::to_string(errno) + " " + strerror(errno));

        if (!S_ISLNK(st.st_mode))
            throw std::invalid_argument("Error: Not a symlink");

        std::unique_ptr<char> buffer = std::make_unique<char>(PATH_MAX + 1);
        ssize_t target_length = readlink(source.c_str(), buffer.get(), PATH_MAX);
        if (target_length == -1)
            throw std::invalid_argument("Error: " + std::to_string(errno) + " " + strerror(errno));
        buffer.get()[target_length] = 0; // terminate string

        return buffer.get();
    }
}
