#include "steam_utils.hpp"

#include <exception>
#include <stdexcept>

#include <fmt/format.h>

#include "filesystem_utils.hpp"
#include "std_utils.hpp"
#include "string_utils.hpp"
#include "vdf.hpp"

#include "exceptions/directory_not_found.hpp"
#include "exceptions/steam_install_not_found.hpp"
#include "exceptions/steam_workshop_directory_not_found.hpp"

#include "static_todo.hpp"

using namespace StringUtils;

using std::filesystem::exists;
using std::filesystem::path;

namespace fs = FilesystemUtils;

SteamUtils::SteamUtils(std::vector<path> search_paths)
{
    steam_path_ = "";
    for (auto const &search_path : search_paths)
    {
        path replace_var = Replace(search_path.c_str(), "$HOME", getenv("HOME"));
        std::string final_path = replace_var / config_path_;
        if (fs::Exists(final_path))
        {
            steam_path_ = replace_var;
            break;
        }
    }
    if (steam_path_.empty())
        throw SteamInstallNotFoundException();
}

path const &SteamUtils::GetSteamPath() const noexcept
{
    return steam_path_;
}

std::vector<path> SteamUtils::GetInstallPaths() const
{
    std::vector<std::filesystem::path> ret;
    ret.emplace_back(steam_path_);

    VDF vdf;
    vdf.LoadFromText(StdUtils::FileReadAllText(steam_path_ / config_path_));

    for (auto const &key : vdf.GetValuesWithFilter("BaseInstallFolder"))
        ret.emplace_back(key);

    return ret;
}

path SteamUtils::GetGamePathFromInstallPath(path const &install_path, std::string const &appid) const
{
    std::filesystem::path manifest_file = install_path / "steamapps" / ("appmanifest_" + appid + ".acf");

    VDF vdf;
    vdf.LoadFromText(StdUtils::FileReadAllText(manifest_file));
    return install_path / "steamapps/common" / vdf.KeyValue["AppState/installdir"];
}

path SteamUtils::GetWorkshopPath(path const &install_path, std::string const &appid) const
{
    path proposed_path = install_path / "steamapps/workshop/content" / appid;
    if (fs::Exists(proposed_path))
        return proposed_path;

    throw SteamWorkshopDirectoryNotFoundException(appid);
}

std::pair<std::filesystem::path, std::string> SteamUtils::GetCompatibilityToolForAppId(std::uint64_t const app_id)
{
    TODO_BEFORE(05, 2020, "Refactor this");
    auto const config_vdf_path = steam_path_ / "config/config.vdf";
    auto const key = fmt::format("InstallConfigStore/Software/Valve/Steam/CompatToolMapping/{}/name", app_id);

    VDF config_vdf;
    config_vdf.LoadFromText(StdUtils::FileReadAllText(config_vdf_path));
    if (!StdUtils::ContainsKey(config_vdf.KeyValue, key))
        throw std::runtime_error("compatibility tool entry not found");

    // we need to scan compat_log.txt for something like "proton_5" and get its appid
    auto const compatibility_tool_shortname = config_vdf.KeyValue[key]; // "proton_5"

    auto const log_file_path = steam_path_ / "logs/compat_log.txt";
    auto const log_content = StdUtils::FileReadAllText(log_file_path);

    auto const text_to_look_for = fmt::format("Registering tool {}, AppID ", compatibility_tool_shortname);
    auto const position = log_content.rfind(text_to_look_for);
    if (position == std::string::npos)
        throw std::runtime_error(fmt::format("cannot find entry for '{}' compatibility tool", compatibility_tool_shortname));

    // expecting somethingth like "Registering tool proton_5, AppID 12345678\n<nextlogline>"
    auto const app_id_start = position + text_to_look_for.length();
    auto const app_id_end = log_content.find_first_of("\r\n[", app_id_start);
    auto const app_id_str = log_content.substr(app_id_start, app_id_end - app_id_start);

    std::filesystem::path compatibility_tool_path;
    if (app_id_str == "0") // custom compatiblity tool installed in ~Steam/compatibilitytools.d/
    {
        TODO_BEFORE(05, 2020, "Write tests for this");
        compatibility_tool_path = GetSteamPath() / "compatibilitytools.d" / compatibility_tool_shortname;
        if (!fs::Exists(compatibility_tool_path))
            throw DirectoryNotFoundException(compatibility_tool_path);
    }
    else // compatibility tool provided by Steam
    {
        for (auto const &install_path : GetInstallPaths())
        {
            try
            {
                compatibility_tool_path = GetGamePathFromInstallPath(install_path, app_id_str);
                break;
            }
            catch (std::exception const &)
            {
            }
        }
    }

    if (compatibility_tool_path.empty())
        throw std::runtime_error(fmt::format("cannot find tool with appid '{}', name '{}'", app_id_str,
                                             compatibility_tool_shortname));

    std::string const command_line_key = "manifest/commandline";
    auto const tool_manifest = compatibility_tool_path / "toolmanifest.vdf";
    VDF tool_manifest_vdf;
    tool_manifest_vdf.LoadFromText(StdUtils::FileReadAllText(tool_manifest));
    if (!StdUtils::ContainsKey(tool_manifest_vdf.KeyValue, command_line_key))
        throw std::runtime_error(fmt::format("cannot read '{}' from '{}'", command_line_key, tool_manifest.string()));

    auto const tool_name = tool_manifest_vdf.KeyValue["manifest/commandline"];
    auto const separator = tool_name.find(' ');
    auto const tool_args = StringUtils::trim(tool_name.substr(separator));
    auto const tool_path = StringUtils::trim(tool_name.substr(0, separator));
    auto full_path = compatibility_tool_path;
    full_path += tool_path;
    return std::pair<std::filesystem::path, std::string>(full_path, tool_args);
}

std::filesystem::path SteamUtils::GetInstallPathFromGamePath(std::filesystem::path const &game_path)
{
    //game_path == "~/.local/share/Steam/steamapps/common/Arma 3"
    return std::filesystem::weakly_canonical(game_path / "../../../");
}
