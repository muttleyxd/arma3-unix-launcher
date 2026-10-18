#pragma once

#include <string_view>

namespace ModSearch
{
    // True if every whitespace separated word of query is found, case-insensitively, in name or
    // workshop_id. An empty or blank query matches every mod.
    bool Matches(std::string_view const name, std::string_view const workshop_id, std::string_view const query);
}
