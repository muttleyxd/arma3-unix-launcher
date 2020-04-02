#include "steam_utils.hpp"

#include <exception>
#include <stdexcept>

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
