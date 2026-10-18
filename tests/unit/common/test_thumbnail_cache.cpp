#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <fstream>
#include <random>
#include <sstream>

#include "default_test_reporter.hpp"

#include "thumbnail_cache.hpp"

using namespace std::chrono_literals;

namespace
{
    struct TemporaryDirectory
    {
        TemporaryDirectory()
        {
            path = std::filesystem::temp_directory_path() / ("a3ul-cache-test-" + std::to_string(std::random_device()()));
            std::filesystem::create_directories(path);
        }
        ~TemporaryDirectory()
        {
            std::error_code ignored;
            std::filesystem::permissions(path, std::filesystem::perms::owner_all, ignored);
            std::filesystem::remove_all(path, ignored);
        }
        std::filesystem::path path;
    };

    std::string const png("\x89PNG\r\n\x1a\n\0\0\0\rIHDR", 16);
    std::string const jpeg("\xff\xd8\xff\xe0\0\x10JFIF\0", 11);
    std::string const gif("GIF89a\x01\0\x01\0", 10);
    std::string const webp("RIFF\x24\0\0\0WEBPVP8 ", 16);

    std::string read_file(std::filesystem::path const &path)
    {
        std::ifstream file(path, std::ios::binary);
        std::stringstream content;
        content << file.rdbuf();
        return content.str();
    }

    // Clock running offset ahead of real time, file times are real
    ThumbnailCache::Clock clock_ahead_by(std::chrono::system_clock::duration const offset)
    {
        return [offset]()
        {
            return std::chrono::system_clock::now() + offset;
        };
    }
}

TEST_CASE("DefaultRoot")
{
    CHECK_EQ(ThumbnailCache::DefaultRoot(),
             std::filesystem::path(getenv("HOME")) / "Library/Caches/a3unixlauncher");
}

TEST_CASE("Lookup_Missing")
{
    TemporaryDirectory directory;
    ThumbnailCache cache(directory.path);
    CHECK_FALSE(cache.Lookup(463939057).has_value());
}

TEST_CASE("Store_DetectsFormatAndLookupFindsIt")
{
    TemporaryDirectory directory;
    ThumbnailCache cache(directory.path);

    for (auto const &[id, bytes, extension] :
            {
                std::tuple{1ull, png, ".png"}, std::tuple{2ull, jpeg, ".jpg"},
                std::tuple{3ull, gif, ".gif"}, std::tuple{4ull, webp, ".webp"}
            })
    {
        CAPTURE(id);
        auto const expected = directory.path / "thumbnails" / (std::to_string(id) + extension);
        CHECK_EQ(cache.Store(id, bytes), expected);
        CHECK_EQ(read_file(expected), bytes);

        auto const entry = cache.Lookup(id);
        REQUIRE(entry.has_value());
        CHECK_EQ(entry->path, expected);
        CHECK(entry->fresh);
    }
}

TEST_CASE("Store_RejectsNonImages")
{
    TemporaryDirectory directory;
    ThumbnailCache cache(directory.path);

    for (std::string const &bytes :
            {
                std::string(), std::string("<html>502 Bad Gateway</html>"), std::string("\x89PN", 3),
                std::string("RIFF\x24\0\0\0WAVE", 12)
            })
    {
        CHECK_FALSE(cache.Store(5, bytes).has_value());
        CHECK_FALSE(cache.Lookup(5).has_value());
    }
}

TEST_CASE("Store_ReplacesPreviousImageInOtherFormat")
{
    TemporaryDirectory directory;
    ThumbnailCache cache(directory.path);

    REQUIRE(cache.Store(7, png).has_value());
    REQUIRE(cache.Store(7, jpeg).has_value());

    auto const entry = cache.Lookup(7);
    REQUIRE(entry.has_value());
    CHECK_EQ(entry->path.filename(), "7.jpg");
    CHECK_FALSE(std::filesystem::exists(directory.path / "thumbnails" / "7.png"));
    CHECK_EQ(std::distance(std::filesystem::directory_iterator(directory.path / "thumbnails"),
                           std::filesystem::directory_iterator()), 1);
}

TEST_CASE("Lookup_FreshUntilMaxAge")
{
    TemporaryDirectory directory;
    REQUIRE(ThumbnailCache(directory.path).Store(9, png).has_value());

    auto const six_days_later = ThumbnailCache(directory.path, 24h * 7, clock_ahead_by(24h * 6)).Lookup(9);
    REQUIRE(six_days_later.has_value());
    CHECK(six_days_later->fresh);

    auto const eight_days_later = ThumbnailCache(directory.path, 24h * 7, clock_ahead_by(24h * 8)).Lookup(9);
    REQUIRE(eight_days_later.has_value());
    CHECK_FALSE(eight_days_later->fresh);
    CHECK_EQ(eight_days_later->path.filename(), "9.png");
}

TEST_CASE("Lookup_IgnoresEmptyAndUnrelatedFiles")
{
    TemporaryDirectory directory;
    std::filesystem::create_directories(directory.path / "thumbnails");
    std::ofstream(directory.path / "thumbnails" / "11.png");                    // empty, e.g. interrupted write
    std::ofstream(directory.path / "thumbnails" / "12.txt") << "not an image";  // unknown extension
    std::ofstream(directory.path / "thumbnails" / "112.png") << png;           // different id with same prefix

    ThumbnailCache cache(directory.path);
    CHECK_FALSE(cache.Lookup(11).has_value());
    CHECK_FALSE(cache.Lookup(12).has_value());
    CHECK_FALSE(cache.Lookup(1).has_value());
}

TEST_CASE("Store_UnwritableDirectory_ReturnsNullopt")
{
    TemporaryDirectory directory;
    std::filesystem::permissions(directory.path, std::filesystem::perms::owner_read | std::filesystem::perms::owner_exec);

    ThumbnailCache cache(directory.path);
    CHECK_FALSE(cache.Store(13, png).has_value());
    CHECK_FALSE(cache.Lookup(13).has_value());
}

TEST_CASE("Store_RootIsAFile_ReturnsNullopt")
{
    TemporaryDirectory directory;
    std::ofstream(directory.path / "file") << "x";

    ThumbnailCache cache(directory.path / "file");
    CHECK_FALSE(cache.Store(14, png).has_value());
    CHECK_FALSE(cache.Lookup(14).has_value());
}
