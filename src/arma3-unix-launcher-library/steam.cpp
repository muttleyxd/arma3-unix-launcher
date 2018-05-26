#include "steam.hpp"

#include <exception>
#include <stdexcept>

#include "filesystem.hpp"
#include "string_utils.hpp"
#include "vdf.hpp"

using namespace Filesystem;
using namespace StringUtils;

Steam::Steam(std::vector<std::string> search_paths)
{
    steam_path_ = "";
    for (std::string &path : search_paths)
    {
        std::string replace_var = Replace(path, "$HOME", getenv("HOME"));
        std::string final_path = replace_var + config_path_;
        if (FileExists(final_path))
        {
            steam_path_ = replace_var;
            break;
        }
    }
    if (steam_path_.empty())
        throw std::invalid_argument("Steam::Steam() - Cannot find Steam install");
}

const std::string &Steam::GetSteamPath() noexcept
{
    return steam_path_;
}

std::vector<std::string> Steam::GetInstallPaths()
{
    std::vector<std::string> ret;
    ret.emplace_back(steam_path_);

    VDF vdf;
    vdf.LoadFromFile(steam_path_ + config_path_);
    for (auto &key : vdf.GetValuesWithFilter("BaseInstallFolder"))
        ret.push_back(key);

    return ret;
}
