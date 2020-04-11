#include "steam_utils.hpp"

#include <exception>
#include <stdexcept>

#include <fmt/format.h>

#include "filesystem_utils.hpp"
#include "std_utils.hpp"
#include "string_utils.hpp"
#include "vdf.hpp"

#include "exceptions/steam_install_not_found.hpp"
#include "exceptions/steam_workshop_directory_not_found.hpp"

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

std::uint64_t SteamUtils::GetCompatibilityToolForAppId(std::uint64_t const app_id)
{
    auto const config_vdf_path = steam_path_ / "config/config.vdf";
    auto const key = fmt::format("InstallConfigStore/Software/Valve/Steam/CompatToolMapping/{}/name", app_id);

    VDF vdf;
    vdf.LoadFromText(StdUtils::FileReadAllText(config_vdf_path));
    if (!StdUtils::ContainsKey(vdf.KeyValue, key))
        return 0;

    // we need to scan compat_log.txt for something like "proton_5" and get its appid
    auto const compatibility_tool_shortname = vdf.KeyValue[key]; // "proton_5"

    auto const log_file_path = steam_path_ / "logs/compat_log.txt";
    auto const log_content = StdUtils::FileReadAllText(log_file_path);

    auto const text_to_look_for = fmt::format("Registering tool {}, AppID ", compatibility_tool_shortname);
    auto const position = log_content.rfind(text_to_look_for);
    if (position == std::string::npos)
        return 0;

    // expecting somethingth like "Registering tool proton_5, AppID 12345678\n<nextlogline>"
    auto const app_id_start = position + text_to_look_for.length();
    auto const app_id_end = log_content.find_first_of("\r\n[", app_id_start);
    auto const app_id_str = log_content.substr(app_id_start, app_id_end - app_id_start);
    return std::stoull(app_id_str);
}

std::filesystem::path SteamUtils::GetInstallPathFromGamePath(std::filesystem::path const &game_path)
{
    //game_path == "~/.local/share/Steam/steamapps/common/Arma 3"
    return std::filesystem::weakly_canonical(game_path / "../../../");
}
