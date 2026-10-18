#include "thumbnail_cache.hpp"

#include <array>
#include <cstdlib>
#include <fstream>
#include <string>
#include <system_error>

namespace
{
    constexpr std::array<char const *, 4> extensions{".png", ".jpg", ".gif", ".webp"};

    bool starts_with(std::string_view const bytes, std::string_view const prefix, size_t const offset = 0)
    {
        return bytes.size() >= offset + prefix.size() && bytes.substr(offset, prefix.size()) == prefix;
    }

    // preview_url has no file extension, so the format comes from the file signature
    char const *detect_extension(std::string_view const bytes)
    {
        if (starts_with(bytes, "\x89PNG\r\n\x1a\n"))
            return ".png";
        if (starts_with(bytes, "\xff\xd8\xff"))
            return ".jpg";
        if (starts_with(bytes, "GIF87a") || starts_with(bytes, "GIF89a"))
            return ".gif";
        if (starts_with(bytes, "RIFF") && starts_with(bytes, "WEBP", 8))
            return ".webp";
        return nullptr;
    }

    std::filesystem::path file_name(std::uint64_t const id, char const *const extension)
    {
        return std::to_string(id) + extension;
    }
}

ThumbnailCache::ThumbnailCache(std::filesystem::path root, std::chrono::hours max_age, Clock now)
    : directory_(std::move(root) / "thumbnails")
    , max_age_(max_age)
    , now_(std::move(now))
{
}

std::filesystem::path ThumbnailCache::DefaultRoot()
{
    char const *const home = std::getenv("HOME");
    std::error_code error;
    std::filesystem::path const base = home ? std::filesystem::path(home) : std::filesystem::temp_directory_path(error);
    return base / "Library/Caches/a3unixlauncher";
}

std::optional<ThumbnailCache::Entry> ThumbnailCache::Lookup(std::uint64_t const id) const
{
    for (auto const extension : extensions)
    {
        auto const path = directory_ / file_name(id, extension);
        std::error_code error;
        auto const size = std::filesystem::file_size(path, error);
        if (error || size == 0)
            continue;

        auto const modified = std::filesystem::last_write_time(path, error);
        if (error)
            continue;
        auto const age = now_() - std::chrono::file_clock::to_sys(modified);
        return Entry{path, age < max_age_};
    }
    return std::nullopt;
}

std::optional<std::filesystem::path> ThumbnailCache::Store(std::uint64_t const id, std::string_view const image_bytes)
{
    auto const extension = detect_extension(image_bytes);
    if (!extension)
        return std::nullopt;

    std::error_code error;
    std::filesystem::create_directories(directory_, error);
    if (error)
        return std::nullopt;

    // Write to a temporary file and rename it, so readers never see a partially written image
    auto const path = directory_ / file_name(id, extension);
    auto temporary_path = path;
    temporary_path += ".tmp";
    {
        std::ofstream file(temporary_path, std::ios::binary | std::ios::trunc);
        file.write(image_bytes.data(), static_cast<std::streamsize>(image_bytes.size()));
        if (!file.good())
        {
            std::filesystem::remove(temporary_path, error);
            return std::nullopt;
        }
    }
    std::filesystem::rename(temporary_path, path, error);
    if (error)
    {
        std::filesystem::remove(temporary_path, error);
        return std::nullopt;
    }

    for (auto const other_extension : extensions)
        if (other_extension != extension)
            std::filesystem::remove(directory_ / file_name(id, other_extension), error);
    return path;
}
