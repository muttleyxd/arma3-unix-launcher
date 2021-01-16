#pragma once

#include <iostream>

#include "mod.hpp"

namespace ARMA3::HtmlPresetExport
{
    std::string export_mods(std::string_view preset_name, std::vector<Mod> const &mods,
                            std::filesystem::path const &workshop_path);
}
