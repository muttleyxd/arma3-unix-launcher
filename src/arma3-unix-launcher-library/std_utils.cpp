#include "std_utils.hpp"

namespace StdUtils
{
    bool Contains(const std::vector<std::string> &vec, const char *t)
    {
        return std::any_of(vec.begin(), vec.end(), std::bind2nd(std::equal_to<std::string>(), std::string(t)));
    }

    bool CreateFile(const std::filesystem::path& path)
    {
        return creat(path.c_str(), 0644);
    }

    std::vector<std::string> Ls(const std::filesystem::path &path, bool set_lowercase)
    {
        std::vector<std::string> ret;
        for (const auto &entity : std::filesystem::directory_iterator(path))
        {
            if (set_lowercase)
                ret.emplace_back(StringUtils::Lowercase(entity.path().filename()));
            else
                ret.emplace_back(entity.path().filename());
        }
        return ret;
    }

    std::string FileReadAllText(const std::filesystem::path &path)
    {
        int fd = open(path.c_str(), 0);
        if (fd == -1)
            throw PathNoAccessException(std::string(path) + "\nError: " + std::to_string(errno) + " " + strerror(errno));
        off_t file_size = lseek(fd, 0, SEEK_END);
        lseek(fd, 0, SEEK_SET);

        std::unique_ptr<char[]> buffer = std::make_unique<char[]>(file_size);
        read(fd, buffer.get(), file_size);
        return buffer.get();
    }
}
