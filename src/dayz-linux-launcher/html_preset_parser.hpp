#pragma once

#include <nlohmann/json.hpp>

namespace Html::Preset::Parser
{
    nlohmann::json html_to_json(std::string const &html_text);
}
