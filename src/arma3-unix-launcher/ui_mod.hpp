#pragma once

#include <string>

#include "string_utils.hpp"

struct UiMod
{
    bool enabled;
    std::string name;
    std::string path_or_workshop_id;
    bool is_workshop_mod;

    char const *is_workshop_mod_to_string() const
    {
        if (is_workshop_mod)
            return "workshop";
        return "custom";
    }
};
