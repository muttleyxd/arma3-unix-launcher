#include "filesystem_utils.hpp"

#include "string_utils.hpp"

namespace FilesystemUtils
{
    bool CreateDirectories(std::filesystem::path const &path)
    {
        return std::filesystem::create_directories(path);
    }

    std::filesystem::path CurrentPath()
    {
        return std::filesystem::current_path();
    }

    void CurrentPath(std::filesystem::path const &path)
    {
        std::filesystem::current_path(path);
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

    uintmax_t RemoveAll(std::filesystem::path const &path)
    {
        return std::filesystem::remove_all(path);
    }

    std::filesystem::path TempDirectoryPath()
    {
        return std::filesystem::temp_directory_path();
    }
}
