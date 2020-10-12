#pragma once

#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include <trompeloeil.hpp>

#include <filesystem>
#include <string>
#include <vector>

class StdUtilsMock
{
    public:
        StdUtilsMock()
        {
            instance = this;
        }
        ~StdUtilsMock()
        {
            instance = nullptr;
        }
        static inline StdUtilsMock *instance;

        MAKE_MOCK1(CreateFile, bool(std::filesystem::path const &));
        MAKE_MOCK1(ExecuteCommand, (std::pair<int, std::string> (std::string_view const)));
        MAKE_MOCK1(FileReadAllText, std::string(std::filesystem::path const &));
        MAKE_MOCK2(FileWriteAllText, void(std::filesystem::path const &, std::string const &));
        MAKE_MOCK1(IsProcessRunning, pid_t(std::string const &));
        MAKE_MOCK2(StartBackgroundProcess, void(std::string const &, std::string_view const));
        MAKE_MOCK1(GetConfigFilePath, std::filesystem::path(std::filesystem::path const &));
        MAKE_MOCK1(IsLibraryAvailable, bool(char const *));
};

namespace StdUtils
{
    bool CreateFile(std::filesystem::path const &path)
    {
        return StdUtilsMock::instance->CreateFile(path);
    }

    std::pair<int, std::string> ExecuteCommand(std::string_view const command)
    {
        return StdUtilsMock::instance->ExecuteCommand(command);
    }

    std::string FileReadAllText(std::filesystem::path const &path)
    {
        return StdUtilsMock::instance->FileReadAllText(path);
    }

    void FileWriteAllText(std::filesystem::path const &path, std::string const &text)
    {
        StdUtilsMock::instance->FileWriteAllText(path, text);
    }

    pid_t IsProcessRunning(std::string const &name)
    {
        return StdUtilsMock::instance->IsProcessRunning(name);
    }

    void StartBackgroundProcess(std::string const &command, std::string_view const working_directory)
    {
        StdUtilsMock::instance->StartBackgroundProcess(command, working_directory);
    }

    std::filesystem::path GetConfigFilePath(std::filesystem::path const &config_filename)
    {
        return StdUtilsMock::instance->GetConfigFilePath(config_filename);
    }

    bool IsLibraryAvailable(char const *const library_filename)
    {
        return StdUtilsMock::instance->IsLibraryAvailable(library_filename);
    }
}
