#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace WorkshopLinks
{
    // Workshop item page in the Steam client, std::nullopt if workshop_id is not a valid item id
    std::optional<std::string> SteamClientUrl(std::string_view const workshop_id);

    // Workshop item page on steamcommunity.com, std::nullopt if workshop_id is not a valid item id
    std::optional<std::string> BrowserUrl(std::string_view const workshop_id);
}
