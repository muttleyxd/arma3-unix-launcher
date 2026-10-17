#include "workshop_links.hpp"

#include <charconv>
#include <cstdint>

#include <fmt/format.h>

namespace
{
    // Workshop item ids are non-zero 64 bit unsigned integers
    std::optional<std::uint64_t> parse_workshop_id(std::string_view const workshop_id)
    {
        std::uint64_t id = 0;
        auto const begin = workshop_id.data();
        auto const end = begin + workshop_id.size();
        auto const [parsed_until, error] = std::from_chars(begin, end, id);
        if (workshop_id.empty() || error != std::errc() || parsed_until != end || id == 0)
            return std::nullopt;
        return id;
    }
}

namespace WorkshopLinks
{
    std::optional<std::string> SteamClientUrl(std::string_view const workshop_id)
    {
        auto const id = parse_workshop_id(workshop_id);
        if (!id)
            return std::nullopt;
        return fmt::format("steam://url/CommunityFilePage/{}", *id);
    }

    std::optional<std::string> BrowserUrl(std::string_view const workshop_id)
    {
        auto const id = parse_workshop_id(workshop_id);
        if (!id)
            return std::nullopt;
        return fmt::format("https://steamcommunity.com/sharedfiles/filedetails/?id={}", *id);
    }
}
