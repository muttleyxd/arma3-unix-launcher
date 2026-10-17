#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

// Steam Web API ISteamRemoteStorage/GetPublishedFileDetails, which needs no API key
namespace WorkshopDetails
{
    constexpr char const *endpoint = "https://api.steampowered.com/ISteamRemoteStorage/GetPublishedFileDetails/v1/";
    constexpr std::size_t max_ids_per_request = 100;

    // application/x-www-form-urlencoded POST bodies for the given ids, at most batch_size ids each
    std::vector<std::string> BuildRequestBodies(std::vector<std::uint64_t> const &ids,
            std::size_t batch_size = max_ids_per_request);
}
