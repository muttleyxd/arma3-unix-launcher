#pragma once

#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include <trompeloeil.hpp>

#include <filesystem>
#include <string>
#include <vector>

class FilesystemUtilsMock
{
    public:
        FilesystemUtilsMock()
        {
            instance = this;
        }
        ~FilesystemUtilsMock()
        {
            instance = nullptr;
        }
        static inline FilesystemUtilsMock *instance;

        MAKE_MOCK1(CreateDirectories, bool(std::filesystem::path const &));
        MAKE_MOCK0(CurrentPath, std::filesystem::path());
        MAKE_MOCK1(CurrentPath, void(std::filesystem::path const &));
        MAKE_MOCK1(Exists, bool(std::filesystem::path const &));
        MAKE_MOCK1(IsDirectory, bool(std::filesystem::path const &));
        MAKE_MOCK2(Ls, std::vector<std::string>(std::filesystem::path const &, bool));
        MAKE_MOCK1(RealPath, std::filesystem::path(std::filesystem::path const &));
        MAKE_MOCK1(RemoveAll, uintmax_t(std::filesystem::path const &));
        MAKE_MOCK0(TempDirectoryPath, std::filesystem::path());

        /*bool CreateDirectories(std::filesystem::path const &path);
        std::filesystem::path CurrentPath();
        void CurrentPath(std::filesystem::path const &path);
        bool Exists(std::filesystem::path const &path);
        bool IsDirectory(std::filesystem::path const &path);
        std::vector<std::string> Ls(std::filesystem::path const &path, bool set_lowercase = false);
        std::filesystem::path RealPath(std::filesystem::path const& path);
        uintmax_t RemoveAll(std::filesystem::path const &path);
        std::filesystem::path TempDirectoryPath();*/
};

namespace FilesystemUtils
{
    bool CreateDirectories(std::filesystem::path const &path)
    {
        return FilesystemUtilsMock::instance->CreateDirectories(path);
    }

    std::filesystem::path CurrentPath()
    {
        return FilesystemUtilsMock::instance->CurrentPath();
    }

    void CurrentPath(std::filesystem::path const &path)
    {
        FilesystemUtilsMock::instance->CurrentPath(path);
    }

    bool Exists(std::filesystem::path const &path)
    {
        return FilesystemUtilsMock::instance->Exists(path);
    }

    bool IsDirectory(std::filesystem::path const &path)
    {
        return FilesystemUtilsMock::instance->IsDirectory(path);
    }

    std::vector<std::string> Ls(std::filesystem::path const &path, bool set_lowercase)
    {
        return FilesystemUtilsMock::instance->Ls(path, set_lowercase);
    }

    std::filesystem::path RealPath(std::filesystem::path const &path)
    {
        return FilesystemUtilsMock::instance->RealPath(path);
    }

    uintmax_t RemoveAll(std::filesystem::path const &path)
    {
        return FilesystemUtilsMock::instance->RemoveAll(path);
    }

    std::filesystem::path TempDirectoryPath()
    {
        return FilesystemUtilsMock::instance->TempDirectoryPath();
    }
}
