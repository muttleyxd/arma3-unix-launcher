#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

// Steam Web API ISteamRemoteStorage/GetPublishedFileDetails, which needs no API key
namespace WorkshopDetails
{
    constexpr char const *endpoint = "https://api.steampowered.com/ISteamRemoteStorage/GetPublishedFileDetails/v1/";
    constexpr std::size_t max_ids_per_request = 100;

    struct ItemDetails
    {
        std::uint64_t id;
        std::string title;
        std::string preview_url;
        std::int64_t time_updated; // unix time, 0 if unknown

        bool operator==(ItemDetails const &) const = default;
    };

    // application/x-www-form-urlencoded POST bodies for the given ids, at most batch_size ids each
    std::vector<std::string> BuildRequestBodies(std::vector<std::uint64_t> const &ids,
            std::size_t batch_size = max_ids_per_request);

    // Items with a preview image from a response body. Items Steam could not resolve (result != 1),
    // without preview_url or with unexpected types are skipped; malformed JSON gives an empty list.
    // Never throws.
    std::vector<ItemDetails> ParseResponse(std::string_view const response_body);
}
