#include "preset_loading_failed.hpp"

PresetLoadingFailedException::PresetLoadingFailedException(std::string const &error) :
    message("Preset loading failed, cannot parse either as JSON or XML\n" +
            error)
{
}

char const *PresetLoadingFailedException::what() const noexcept
{
    return message.c_str();
}
