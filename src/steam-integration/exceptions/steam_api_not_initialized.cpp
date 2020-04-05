#include "steam_api_not_initialized.hpp"

char const *SteamApiNotInitializedException::what() const noexcept
{
    return "Steam API is not initalized";
}
