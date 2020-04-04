#include "steam_api_failed.hpp"

SteamApiFailedException::SteamApiFailedException(std::string const &error): message("Steam API call failed: " + error)
{
}

char const *SteamApiFailedException::what() const noexcept
{
    return message.c_str();
}
