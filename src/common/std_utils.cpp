#include "std_utils.hpp"

#include <fmt/format.h>
#include <fstream>
#include <filesystem>

extern "C" {
#include <dlfcn.h>
#include <unistd.h>
}

namespace StdUtils
{
    bool CreateFile(std::filesystem::path const &path)
    {
        auto result = open(path.c_str(), O_CREAT | O_WRONLY, 0644);
        if (result == -1)
            return false;
        close(result);
        return true;
    }

    std::pair<int, std::string> ExecuteCommand(std::string_view const command)
    {
        std::array<char, 128> buffer{};
        int exit_code = -1;
        std::string output;

        {
            // local scope kicks off pclose before returning exit_code
            auto deleter = [&exit_code](std::FILE * ptr)
            {
                exit_code = pclose(ptr);
            };
            std::unique_ptr<std::FILE, decltype(deleter)> pipe(popen(command.data(), "r"), deleter);

            if (!pipe)
                throw std::runtime_error("popen() failed!");

            while (std::fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
                output += buffer.data();
        }

        return {exit_code, output};
    }


    std::string FileReadAllText(std::filesystem::path const &path)
    {
        std::ifstream file(path);
        std::stringstream str;
        str << file.rdbuf();
        return str.str();
    }

    void FileWriteAllText(std::filesystem::path const &path, std::string const &text)
    {
        if (!exists(path.parent_path()))
            throw std::filesystem::filesystem_error("Parent dir does not exist", {});
        std::ofstream(path) << text;
    }

    pid_t IsProcessRunning(std::string const &name, bool case_insensitive)
    {
        #ifdef __linux
        for (auto const &entity : std::filesystem::directory_iterator("/proc"))
        {
            try
            {
                if (entity.is_symlink() || !entity.is_directory() || !exists(entity.path() / "exe"))
                    continue;

                std::filesystem::path exe_path = read_symlink(entity.path() / "exe");

                using StringUtils::Lowercase;
                using StringUtils::trim;

                if (case_insensitive)
                {
                    if (Lowercase(exe_path.filename()) == Lowercase(name))
                        return std::stoi(entity.path().filename());
                    else if (exe_path.filename() == "wine64-preloader"
                             && trim(Lowercase(FileReadAllText(entity.path() / "comm"))) == Lowercase(name))
                        return std::stoi(entity.path().filename());
                }
                else
                {
                    if (exe_path.filename() == name)
                        return std::stoi(entity.path().filename());
                    else if (exe_path.filename() == "wine64-preloader" && trim(FileReadAllText(entity.path() / "comm")) == name)
                        return std::stoi(entity.path().filename());
                }
            }
            catch (std::filesystem::filesystem_error const &)
            {
                // most likely ACCCESS DENIED to other users' processes
            }
        }

        return -1;
        #else
        auto processes = ExecuteCommand("ps -eo pid=,ucomm=");
        if (processes.first != 0)
            return -1;

        auto name_looking_for = name;
        if (StringUtils::EndsWith(name_looking_for, ".app"))
            name_looking_for = name_looking_for.substr(0, name_looking_for.size() - 4);
        if (case_insensitive)
            name_looking_for = StringUtils::Lowercase(name_looking_for);

        for (auto const &line : StringUtils::Split(processes.second, "\n")) // line: 525 ArmA3.App
        {
            auto trimmed = StringUtils::trim(line);
            auto space_index = trimmed.find(' ');
            if (space_index == trimmed.npos)
            {
                fmt::print("Didnt find space in line \"{}\" from ps output\n", trimmed);
                continue;
            }

            auto proc_id = trimmed.substr(0, space_index);
            auto proc_name = trimmed.substr(space_index + 1);

            std::string process_name(proc_name);
            if (case_insensitive)
                process_name = StringUtils::Lowercase(std::string(proc_name));
            if (name_looking_for == process_name)
                return std::stoi(std::string(proc_id));
        }
        return -1;
        #endif
    }

    void StartBackgroundProcess(std::string const &command, std::string_view const working_directory)
    {
        system(fmt::format("bash -c 'cd \"{}\"; {} <&- &\'", working_directory, command).c_str());
    }

    std::filesystem::path GetConfigFilePath(std::filesystem::path const &config_filename)
    {
        std::filesystem::path config_directory = fmt::format("{}/.config/a3unixlauncher", getenv("HOME"));

        auto xdg_config_home = getenv("XDG_CONFIG_HOME");
        if (xdg_config_home)
            config_directory = fmt::format("{}/a3unixlauncher", xdg_config_home);

        return config_directory / config_filename;
    }

    bool IsLibraryAvailable(char const *const library_filename)
    {
        void *handle = dlopen(library_filename, RTLD_LAZY);
        if (handle == nullptr)
            return false;

        dlclose(handle);
        return true;
    }

}
