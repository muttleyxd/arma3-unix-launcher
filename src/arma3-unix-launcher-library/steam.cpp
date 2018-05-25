#include "steam.hpp"

#include <exception>
#include <stdexcept>

#include "filesystem.hpp"
#include "string_utils.hpp"

using namespace Filesystem;
using namespace StringUtils;

Steam::Steam(std::vector<std::string> search_paths)
{
    steam_path_ = "";
    for (std::string &path : search_paths)
    {
        std::string replace_var = Replace(path, "$HOME", getenv("HOME"));
        std::string config_path = "/config/config.vdf";
        std::string final_path = replace_var + config_path;
        if (FileExists(final_path))
        {
            steam_path_ = path;
            break;
        }
    }
    if (steam_path_.empty())
        throw std::invalid_argument("Steam::Steam() - Cannot find Steam install");
}

const std::string& Steam::GetSteamPath() noexcept
{
    return steam_path_;
}

std::vector<std::string> Steam::GetInstallPaths()
{
    return std::vector<std::string>();
}
