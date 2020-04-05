#include "steam_workshop_directory_not_found.hpp"

SteamWorkshopDirectoryNotFoundException::SteamWorkshopDirectoryNotFoundException(std::string const &appid) :
    message("Steam Workshop directory not found for appid: " + appid)
{
}

char const *SteamWorkshopDirectoryNotFoundException::SteamWorkshopDirectoryNotFoundException::what() const noexcept
{
    return message.c_str();
}
