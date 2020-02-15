#include "filesystem_utils.hpp"

#include "string_utils.hpp"

namespace FilesystemUtils
{
    bool CreateDirectories(std::filesystem::path const &path)
    {
        return std::filesystem::create_directories(path);
    }

    bool Exists(std::filesystem::path const &path)
    {
        return std::filesystem::exists(path);
    }

    bool IsDirectory(std::filesystem::path const &path)
    {
        return std::filesystem::is_directory(path);
    }

    std::vector<std::string> Ls(std::filesystem::path const &path, bool set_lowercase)
    {
        std::vector<std::string> ret;
        for (auto const &entity : std::filesystem::directory_iterator(path))
        {
            if (set_lowercase)
                ret.emplace_back(StringUtils::Lowercase(entity.path().filename()));
            else
                ret.emplace_back(entity.path().filename());
        }
        return ret;
    }
}
