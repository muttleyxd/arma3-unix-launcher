#include "exceptions/steam_workshop_directory_not_found.hpp"

SteamWorkshopDirectoryNotFoundException::SteamWorkshopDirectoryNotFoundException(std::string const &appid)
{
    msg_ = "Steam Workshop directory not found for appid: " + appid;
}

const char *SteamWorkshopDirectoryNotFoundException::SteamWorkshopDirectoryNotFoundException::what() const noexcept
{
    return msg_.c_str();
}
