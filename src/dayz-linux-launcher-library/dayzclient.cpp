#include "dayzclient.hpp"

#include <algorithm>
#include <filesystem>
#include <string>

#include <fmt/format.h>
#include <fmt/ostream.h>
#include <spdlog/spdlog.h>

#include <static_todo.hpp>

#include "cppfilter.hpp"
#include "filesystem_utils.hpp"
#include "steam_utils.hpp"
#include "std_utils.hpp"
#include "string_utils.hpp"

#include "exceptions/directory_not_found.hpp"
#include "exceptions/file_no_access.hpp"
#include "exceptions/file_not_found.hpp"
#include "exceptions/not_a_symlink.hpp"
#include "exceptions/syntax_error.hpp"

using namespace std;
using namespace std::filesystem;

using namespace StdUtils;
using namespace StringUtils;

namespace fs = FilesystemUtils;

#ifdef __linux
namespace
{
    std::string get_esync_prefix(bool disable_esync)
    {
        if (disable_esync)
            return "PROTON_NO_ESYNC=1";
        return "";
    }

    std::string optional_steam_runtime(SteamUtils const &steam_utils)
    {
        if (StdUtils::IsLibraryAvailable("libpng12.so"))
            return "";
        auto const steam_runtime_path = steam_utils.GetSteamPath() / "ubuntu12_32/steam-runtime/run.sh";
        if (fs::Exists(steam_runtime_path))
            return steam_runtime_path;
        spdlog::critical("Did not find {} and libpng12.so is missing. Thermal optics will be broken!", steam_runtime_path);
        return "";
    }

    std::string get_verb(std::string const &arguments)
    {
        return StringUtils::Replace(arguments, "%verb%", "run");;
    }

    void direct_launch(std::filesystem::path const &arma_path, std::filesystem::path const &executable_path,
                       string const &arguments, string const &user_environment_variables, bool disable_esync)
    {
        spdlog::trace("{}:{} arma path: '{}', executable path: '{}', arguments: {}, user_environment_variables: {}, disable_esync: {}",
                      __PRETTY_FUNCTION__, __LINE__, arma_path, executable_path, arguments, user_environment_variables,
                      disable_esync);
        try
        {
            SteamUtils steam_utils;
            auto const dayz_id = std::stoull(DayZ::Definitions::app_id);
            auto const compatibility_tool = steam_utils.GetCompatibilityToolForAppId(dayz_id);

            auto ld_preload_path = fmt::format("{}/ubuntu12_64/gameoverlayrenderer.so", steam_utils.GetSteamPath().string());
            if (char const *old_ld_preload = getenv("LD_PRELOAD"); old_ld_preload)
                ld_preload_path = fmt::format("{}:{}", ld_preload_path, old_ld_preload);

            auto const steam_compat_data_path = steam_utils.GetInstallPathFromGamePath(arma_path) / "steamapps/compatdata" /
                                                DayZ::Definitions::app_id;

            auto const environment = fmt::format(R"env({} SteamGameId={} LD_PRELOAD={} STEAM_COMPAT_DATA_PATH="{}")env",
                                                 get_esync_prefix(disable_esync), dayz_id, ld_preload_path, steam_compat_data_path.string());
            auto const command = fmt::format(R"command(env {} {} {} {} {} "{}" {})command", environment, user_environment_variables,
                                             optional_steam_runtime(steam_utils), compatibility_tool.first,
                                             get_verb(compatibility_tool.second), executable_path.string(), arguments);
            spdlog::info("Running DayZ:\n{}\n", command);
            StdUtils::StartBackgroundProcess(command, arma_path.string());
        }
        catch (std::exception const &e)
        {
            spdlog::critical("Direct launch failed, exception: {}", e.what());
        }
    }

    void indirect_flatpak_launch(string const &arguments, string const &user_environment_variables,
                                 bool disable_esync)
    {
        spdlog::trace("{}:{}", __PRETTY_FUNCTION__, __LINE__);
        StdUtils::StartBackgroundProcess(
                fmt::format("flatpak run --env=\"{} {}\" com.valvesoftware.Steam -applaunch 221100 -nolauncher {}",
                            get_esync_prefix(disable_esync), user_environment_variables, arguments));
    }

    void indirect_launch(string const &arguments, string const &user_environment_variables,
                         bool disable_esync)
    {
        spdlog::trace("{}:{}", __PRETTY_FUNCTION__, __LINE__);
        StdUtils::StartBackgroundProcess(fmt::format("env {} {} steam -applaunch 221100 -nolauncher {}",
                                         get_esync_prefix(disable_esync), user_environment_variables, arguments));
    }

    void indirect_launch_through_steam(std::string const &arguments, std::string const &user_environment_variables,
                                       bool disable_esync, bool is_flatpak)
    {
        spdlog::trace("{}:{} is_flatpak: {}",
                      __PRETTY_FUNCTION__, __LINE__, is_flatpak);

        // god knows why it doesn't work with backslashes
        auto const indirect_arguments = StringUtils::Replace(arguments, "@dzmods\\", "@dzmods/");
        if (is_flatpak)
            indirect_flatpak_launch(indirect_arguments, user_environment_variables, disable_esync);
        else
            indirect_launch(indirect_arguments, user_environment_variables, disable_esync);
    }
}
#endif

namespace DayZ
{
    Client::Client(std::filesystem::path const &arma_path, std::filesystem::path const &target_workshop_path)
    {
        path_ = arma_path;
        if (!fs::Exists(path_ / Definitions::executable_name))
            throw FileNotFoundException(Definitions::executable_name);
        path_executable_ = path_ / Definitions::executable_name;
        path_workshop_target_ = target_workshop_path;
    }

    void Client::Start(std::vector<std::filesystem::path> const &mod_paths, std::string const &arguments, std::string const& user_environment_variables, bool launch_directly, bool disable_esync)
    {
        spdlog::trace("{}:{} arguments: '{}', user_environment_variables: '{}', launch_directly: {}, disable_esync: {}",
                      __PRETTY_FUNCTION__, __LINE__, arguments, user_environment_variables, launch_directly, disable_esync);

        auto const local_mod_dir = GetPath() / "@dzmods";
        spdlog::trace("{}:{} local mod dir: {}", __PRETTY_FUNCTION__, __LINE__, local_mod_dir);

        if (FilesystemUtils::Exists(local_mod_dir))
        {
            for (auto const& entry : FilesystemUtils::Ls(local_mod_dir))
                std::filesystem::remove(local_mod_dir / entry);
        }
        else
            std::filesystem::create_directories(local_mod_dir);

        for (std::size_t i = 0; i < mod_paths.size(); ++i)
            std::filesystem::create_directory_symlink(mod_paths[i], fmt::format("{}/{}", local_mod_dir.string(), i));

        std::string arguments_with_mods = fmt::format("{}{}", arguments, GetModParameter(mod_paths.size()));

        if (launch_directly)
            direct_launch(GetPath(), GetPathExecutable(), arguments_with_mods, user_environment_variables, disable_esync);
        else
            indirect_launch_through_steam(arguments_with_mods, user_environment_variables, disable_esync, IsFlatpak());
    }

    std::vector<Mod> Client::GetHomeMods()
    {
        return GetModsFromDirectory(GetPath());
    }

    std::vector<Mod> Client::GetWorkshopMods()
    {
        return GetModsFromDirectory(GetPathWorkshop());
    }

    std::filesystem::path Client::GetCfgPath()
    {
        return GetPath() / Definitions::game_config_path;
    }

    char Client::GetFakeDriveLetter()
    {
        return 'Z';
    }

    bool Client::IsFlatpak()
    {
#ifdef __linux
        try
        {
            SteamUtils steam_utils;
            return steam_utils.IsFlatpak();
        }
        catch (std::exception const &e)
        {
            spdlog::warn("Exception trying to determine if Flatpak, exception text: {}", e.what());
            return false;
        }
#else
        return false;
#endif
    }

    std::string Client::GetModParameter(std::size_t count)
    {
        if (count == 0)
            return "";

        std::string argument = " -mod=";

        for (std::size_t i = 0; i < count; ++i) {
            argument += fmt::format("@dzmods\\\\{}", i);
            if (i < count -1)
                argument += "\\;";
        }

        return argument;
    }

    std::filesystem::path const &Client::GetPath()
    {
        return path_;
    }

    std::filesystem::path const &Client::GetPathExecutable()
    {
        return path_executable_;
    }

    std::filesystem::path const &Client::GetPathWorkshop()
    {
        return path_workshop_target_;
    }

    std::vector<Mod> Client::GetModsFromDirectory(path const &dir)
    {
        std::vector<Mod> ret;

        for (auto const &ent : fs::Ls(dir))
        {
            if (StdUtils::Contains(Definitions::exclusions, ent))
                continue;

            std::filesystem::path mod_dir = dir / ent;
            if (!fs::IsDirectory(mod_dir))
                continue;
            if (!StdUtils::Contains(fs::Ls(mod_dir, true), "addons"))
                continue;

            ret.emplace_back(mod_dir);
        }

        std::sort(ret.begin(), ret.end(), [](Mod const & m1, Mod const & m2)
        {
            return m1.GetValueOrReturnDefault("name", "dir", StringUtils::Lowercase(m1.path_)) < m2.GetValueOrReturnDefault("name", "dir", StringUtils::Lowercase(m2.path_));
        });

        return ret;
    }
}
