#include "arma3client.hpp"

#include <algorithm>
#include <filesystem>
#include <string>

#include <fmt/format.h>
#include <fmt/ostream.h>

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
        fmt::print(stderr, "Did not find {} and libpng12.so is missing. Thermal optics will be broken!\n", steam_runtime_path);
        return "";
    }

    std::string get_verb(std::string const &arguments)
    {
        return StringUtils::Replace(arguments, "%verb%", "run");;
    }

    void direct_launch(std::filesystem::path const &arma_path, std::filesystem::path const &executable_path,
                       string const &arguments, bool is_proton, string const &user_environment_variables, bool disable_esync)
    {
        if (!is_proton)
        {
            StdUtils::StartBackgroundProcess(fmt::format("env {} {} {}", user_environment_variables, executable_path, arguments),
                                             arma_path.string());
            return;
        }

        try
        {
            SteamUtils steam_utils;
            auto const arma3_id = std::stoull(ARMA3::Definitions::app_id);
            auto const compatibility_tool = steam_utils.GetCompatibilityToolForAppId(arma3_id);

            auto ld_preload_path = fmt::format("{}/ubuntu12_64/gameoverlayrenderer.so", steam_utils.GetSteamPath().string());
            if (char const *old_ld_preload = getenv("LD_PRELOAD"); old_ld_preload)
                ld_preload_path = fmt::format("{}:{}", ld_preload_path, old_ld_preload);

            auto const steam_compat_data_path = steam_utils.GetInstallPathFromGamePath(arma_path) / "steamapps/compatdata" /
                                                ARMA3::Definitions::app_id;

            auto const environment = fmt::format(R"env({} SteamGameId={} LD_PRELOAD={} STEAM_COMPAT_DATA_PATH="{}")env",
                                                 get_esync_prefix(disable_esync), arma3_id, ld_preload_path, steam_compat_data_path.string());
            auto const command = fmt::format(R"command(env {} {} {} {} {} "{}" {})command", environment, user_environment_variables,
                                             optional_steam_runtime(steam_utils), compatibility_tool.first,
                                             get_verb(compatibility_tool.second), executable_path.string(), arguments);
            fmt::print("Running Arma:\n{}\n", command);
            StdUtils::StartBackgroundProcess(command, arma_path.string());
        }
        catch (std::exception const &e)
        {
            fmt::print(stderr, "Direct launch failed, exception: {}\n", e.what());
        }
    }

    void indirect_flatpak_launch(string const &arguments, bool is_proton, string const &user_environment_variables,
                                 bool disable_esync)
    {
        if (is_proton)
            StdUtils::StartBackgroundProcess(
                fmt::format("flatpak run --env=\"{} {}\" com.valvesoftware.Steam -applaunch 107410 -nolauncher {}",
                            get_esync_prefix(disable_esync), user_environment_variables, arguments));
        else
            StdUtils::StartBackgroundProcess(fmt::format("flatpak run com.valvesoftware.Steam -applaunch 107410 -nolauncher {}",
                                             arguments));
    }

    void indirect_launch(string const &arguments, bool is_proton, string const &user_environment_variables,
                         bool disable_esync)
    {
        if (is_proton)
            StdUtils::StartBackgroundProcess(fmt::format("env {} {} steam -applaunch 107410 -nolauncher {}",
                                             get_esync_prefix(disable_esync), user_environment_variables, arguments));
        else
            StdUtils::StartBackgroundProcess(fmt::format("env {} steam -applaunch 107410 {}", user_environment_variables,
                                             arguments));
    }

    void indirect_launch_through_steam(string const &arguments, bool is_proton, string const &user_environment_variables,
                                       bool disable_esync, bool is_flatpak)
    {
        if (is_flatpak)
            indirect_flatpak_launch(arguments, is_proton, user_environment_variables, disable_esync);
        else
            indirect_launch(arguments, is_proton, user_environment_variables, disable_esync);
    }
}
#endif

namespace ARMA3
{
    Client::Client(std::filesystem::path const &arma_path, std::filesystem::path const &target_workshop_path)
    {
        path_ = arma_path;
        for (auto const &executable : Definitions::executable_names)
        {
            if (fs::Exists(path_ / executable))
            {
                path_executable_ = path_ / executable;
                break;
            }
        }
        if (path_executable_.empty())
            throw FileNotFoundException("arma3.exe");
        path_workshop_target_ = target_workshop_path;
    }

    void Client::CreateArmaCfg(vector<path> const &mod_paths, path cfg_path)
    {
        if (cfg_path.empty())
            cfg_path = GetCfgPath();
        if (!fs::Exists(cfg_path))
        {
            if (!fs::Exists(cfg_path.parent_path()))
                fs::CreateDirectories(cfg_path.parent_path());
            StdUtils::CreateFile(cfg_path);
        }
        std::string existing_config = FileReadAllText(cfg_path);

        CppFilter cpp_filter{existing_config};
        auto stripped_config = cpp_filter.RemoveClass("class ModLauncherList");
        stripped_config += R"cpp(class ModLauncherList
{
)cpp";

        int mod_number = 1;
        for (auto const& mod_path : mod_paths)
            stripped_config += GenerateCfgCppForMod(mod_path, mod_number++);

        stripped_config += "};\n";

        FileWriteAllText(cfg_path, stripped_config);
    }

    bool Client::IsProton()
    {
        return path_executable_.filename() == "arma3_x64.exe";
    }

    void Client::Start(string const &arguments, string const& user_environment_variables, bool launch_directly, bool disable_esync)
    {
        #ifdef __linux
        if (launch_directly)
            direct_launch(GetPath(), GetPathExecutable(), arguments, IsProton(), user_environment_variables, disable_esync);
        else
            indirect_launch_through_steam(arguments, IsProton(), user_environment_variables, disable_esync, IsFlatpak());
        #else
        (void)disable_esync; // esync does not apply on Mac OS X
        if (launch_directly)
        {
            try
            {
                SteamUtils steam_utils;
                auto ld_preload_path = fmt::format("{}/Steam.AppBundle/Steam/Contents/MacOS/gameoverlayrenderer.dylib", steam_utils.GetSteamPath().string());
                if (char const *old_ld_preload = getenv("DYLD_INSERT_LIBRARIES"); old_ld_preload)
                    ld_preload_path += fmt::format("{}:{}", ld_preload_path, old_ld_preload);
                auto const environment = fmt::format(R"env(DYLD_INSERT_LIBRARIES="{}")env", ld_preload_path);
                auto const command = fmt::format(R"command(env {} {} "{}/Contents/MacOS/ArmA3" {})command", environment, user_environment_variables, GetPathExecutable().string(), arguments);
                fmt::print("Running Arma:\n{}\n", command);
                StdUtils::StartBackgroundProcess(command, GetPath().string());
            }
            catch (std::exception const& e)
            {
                fmt::print(stderr, "Direct launch failed, exception: {}\n", e.what());
            }
        }
        else
        {
            std::string launch_command = fmt::format("env {} open steam://run/107410//" + StringUtils::Replace(arguments, " ", "%20"), user_environment_variables);
            StdUtils::StartBackgroundProcess(launch_command);
        }
        #endif
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
        if (IsProton())
            return GetPath() / Definitions::proton_config_relative_path;
        else if (IsFlatpak())
            return std::filesystem::path(Definitions::home_directory)
                   / Definitions::flatpak_prefix
                   / Definitions::local_share_prefix
                   / Definitions::bohemia_interactive_prefix
                   / Definitions::game_config_path;
        else
            return std::filesystem::path(Definitions::home_directory)
                   / Definitions::local_share_prefix
                   / Definitions::bohemia_interactive_prefix
                   / Definitions::game_config_path;
    }

    char Client::GetFakeDriveLetter()
    {
        return IsProton() ? 'Z' : 'C';
    }

    string Client::GenerateCfgCppForMod(path const &mod_path, int mod_index)
    {
        constexpr char const *mod_template =
            R"cpp(    class Mod{}
    {{
        dir="{}";
        name="{}";
        origin="GAME DIR";
        fullPath="{}";
    }};
)cpp";

        Mod mod(mod_path);
        path mod_path_absolute = trim(mod.path_.string(), "\"");
        path final_path = StringUtils::ToWindowsPath(mod_path_absolute, GetFakeDriveLetter());
        path dir = trim(mod_path_absolute.filename().string(), "\"");
        auto name = mod.GetValueOrReturnDefault(dir, "name", "dir", "tooltip", "name_read_failed");
        name = StringUtils::Replace(name, "\"", "_");

        return fmt::format(mod_template, mod_index, dir.string(), name, final_path.string());
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
            fmt::print(stderr, "Exception trying to determine if Flatpak, exception text: {}\n", e.what());
            return false;
        }
#else
        return false;
#endif
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
