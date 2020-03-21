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
  FilesystemUtilsMock() { instance = this; }
  ~FilesystemUtilsMock() { instance = nullptr; }
  static inline FilesystemUtilsMock* instance;

  MAKE_MOCK1(CreateDirectories, bool(std::filesystem::path const&));
  MAKE_MOCK1(Exists, bool(std::filesystem::path const&));
  MAKE_MOCK1(IsDirectory, bool(std::filesystem::path const&));
  MAKE_MOCK2(Ls, std::vector<std::string>(std::filesystem::path const&, bool));
};

namespace FilesystemUtils
{
    bool CreateDirectories(std::filesystem::path const &path)
    {
        return FilesystemUtilsMock::instance->CreateDirectories(path);
    }

    bool Exists(std::filesystem::path const& path)
    {
        return FilesystemUtilsMock::instance->Exists(path);
    }

    bool IsDirectory(std::filesystem::path const& path)
    {
        return FilesystemUtilsMock::instance->IsDirectory(path);
    }

    std::vector<std::string> Ls(std::filesystem::path const &path, bool set_lowercase)
    {
        return FilesystemUtilsMock::instance->Ls(path, set_lowercase);
    }
}
