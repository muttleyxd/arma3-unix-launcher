#include "workshop_details.hpp"

#include <algorithm>

#include <fmt/format.h>

namespace WorkshopDetails
{
    std::vector<std::string> BuildRequestBodies(std::vector<std::uint64_t> const &ids, std::size_t batch_size)
    {
        batch_size = std::max<std::size_t>(batch_size, 1);

        std::vector<std::string> bodies;
        for (std::size_t batch_start = 0; batch_start < ids.size(); batch_start += batch_size)
        {
            std::size_t const count = std::min(batch_size, ids.size() - batch_start);
            // "publishedfileids[i]", with the brackets percent-encoded
            std::string body = fmt::format("itemcount={}", count);
            for (std::size_t i = 0; i < count; ++i)
                body += fmt::format("&publishedfileids%5B{}%5D={}", i, ids[batch_start + i]);
            bodies.push_back(std::move(body));
        }
        return bodies;
    }
}
