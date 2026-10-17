#include "workshop_details.hpp"

#include <algorithm>
#include <charconv>
#include <optional>

#include <fmt/format.h>
#include <nlohmann/json.hpp>

namespace
{
    // Lookups below check types first: nlohmann::json throws on type mismatches otherwise

    nlohmann::json const *find_member(nlohmann::json const &object, char const *const key)
    {
        if (!object.is_object())
            return nullptr;
        auto const it = object.find(key);
        return it == object.end() ? nullptr : &*it;
    }

    std::optional<std::int64_t> integer_member(nlohmann::json const &object, char const *const key)
    {
        auto const value = find_member(object, key);
        if (!value || !value->is_number_integer())
            return std::nullopt;
        return value->get<std::int64_t>();
    }

    std::string string_member(nlohmann::json const &object, char const *const key)
    {
        auto const value = find_member(object, key);
        if (!value || !value->is_string())
            return {};
        return value->get_ref<std::string const &>();
    }

    // publishedfileid is a decimal string
    std::optional<std::uint64_t> id_member(nlohmann::json const &object)
    {
        auto const text = string_member(object, "publishedfileid");
        std::uint64_t id = 0;
        auto const [end, error] = std::from_chars(text.data(), text.data() + text.size(), id);
        if (text.empty() || error != std::errc() || end != text.data() + text.size() || id == 0)
            return std::nullopt;
        return id;
    }
}

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

    std::vector<ItemDetails> ParseResponse(std::string_view const response_body)
    {
        auto const json = nlohmann::json::parse(response_body, nullptr, false);
        auto const response = find_member(json, "response");
        auto const details = response ? find_member(*response, "publishedfiledetails") : nullptr;
        if (!details || !details->is_array())
            return {};

        std::vector<ItemDetails> items;
        for (auto const &item : *details)
        {
            auto const id = id_member(item);
            auto preview_url = string_member(item, "preview_url");
            if (!id || integer_member(item, "result") != 1 || preview_url.empty())
                continue;
            items.push_back({*id, string_member(item, "title"), std::move(preview_url),
                             integer_member(item, "time_updated").value_or(0)});
        }
        return items;
    }
}
