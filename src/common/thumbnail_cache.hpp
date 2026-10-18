#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <optional>
#include <string_view>

// On-disk cache of Workshop preview images: <root>/thumbnails/<id>.<png|jpg|gif|webp>.
// No method throws, filesystem errors are reported as std::nullopt.
class ThumbnailCache
{
    public:
        using Clock = std::function<std::chrono::system_clock::time_point()>;

        struct Entry
        {
            std::filesystem::path path;
            bool fresh; // stored less than max_age ago; stale entries are still usable, e.g. offline
        };

        explicit ThumbnailCache(std::filesystem::path root,
                                std::chrono::hours max_age = std::chrono::hours(24 * 7),
                                Clock now = std::chrono::system_clock::now);

        // ~/Library/Caches/a3unixlauncher
        static std::filesystem::path DefaultRoot();

        std::optional<Entry> Lookup(std::uint64_t const id) const;

        // Stores image_bytes (PNG, JPEG, GIF or WebP, detected from the content), replacing any
        // previous image for id. Returns the stored file, std::nullopt for other content or on errors.
        std::optional<std::filesystem::path> Store(std::uint64_t const id, std::string_view const image_bytes);

    private:
        std::filesystem::path directory_;
        std::chrono::hours max_age_;
        Clock now_;
};
