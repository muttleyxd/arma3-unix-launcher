#pragma once

#include <filesystem>

#include <fmt/format.h>

template<>
struct fmt::formatter<std::filesystem::path>
{
    template<typename ParseContext>
    constexpr auto parse(ParseContext& context)
    {
        return context.begin();
    }

    template<typename FormatContext>
    auto format(std::filesystem::path const& path, FormatContext& context)
    {
        // replicate fmt 7.x behavior - '/path' gets formatted to '"/path"'
        return format_to(context.begin(), "\"{}\"", path.string());
    }
};

