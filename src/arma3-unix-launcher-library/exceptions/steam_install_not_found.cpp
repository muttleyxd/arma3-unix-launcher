#include "steam_install_not_found.hpp"

char const *SteamInstallNotFoundException::what() const noexcept
{
    return "Steam installation not found";
}
