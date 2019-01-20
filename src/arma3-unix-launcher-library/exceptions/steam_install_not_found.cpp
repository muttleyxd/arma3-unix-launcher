#include "exceptions/steam_install_not_found.hpp"

const char *SteamInstallNotFoundException::what() const noexcept
{
    return "Steam installation not found";
}
