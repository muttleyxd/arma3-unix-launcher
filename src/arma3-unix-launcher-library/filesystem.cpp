#include "filesystem.hpp"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <memory>

namespace Filesystem
{
    int DirectoryCreate(const std::string &path)
    {
        return mkdir(path.c_str(), 0755);
    }

    int DirectoryDelete(const std::string &path, bool recursive)
    {
        if (!recursive)
            return rmdir(path.c_str());
        // Should've been implemented properly
        return system(("rm -rf " + path).c_str());
    }

    int DirectoryExists(const std::string &path)
    {
        struct stat st;
        int result = stat(path.c_str(), &st);

        /*
         * if stat reports error - pass it further
         * if is not directory then return exists but it's not a directory
         * else return it exists
         */
        if (result != 0)
            return result;
        else if (!S_ISDIR(st.st_mode))
        {
            errno = EEXIST;
            return 1;
        }
        else
            return result;
    }

    int FileCreate(const std::string &path, const std::string &content)
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

    int FileDelete(const std::string &path)
    {
        return unlink(path.c_str());
    }

    int FileExists(const std::string &path)
    {
        struct stat st;
        int result = stat(path.c_str(), &st);

        // As in DirectoryExists
        if (result != 0)
            return result;
        else if (S_ISDIR(st.st_mode))
        {
            errno = EEXIST;
            return 1;
        }
        else
            return result;
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

    std::vector<std::string> GetSubdirectories(const std::string &path)
    {
        std::vector<std::string> ret;
        return ret;
    }

    int SymlinkCreate(const std::string &source, const std::string &target)
    {
        return 0;
    }

    std::string SymlinkGetTarget(const std::string &source)
    {
        return "";
    }
}
