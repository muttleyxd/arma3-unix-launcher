#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <map>
#include <random>

#include <fmt/format.h>

#include "default_test_reporter.hpp"
#include "mock/http_client_mock.hpp"

#include "thumbnail_service.hpp"
#include "workshop_details.hpp"

using namespace std::chrono_literals;
using trompeloeil::_;

namespace
{
    struct TemporaryDirectory
    {
        TemporaryDirectory()
        {
            path = std::filesystem::temp_directory_path() / ("a3ul-service-test-" + std::to_string(std::random_device()()));
            std::filesystem::create_directories(path);
        }
        ~TemporaryDirectory()
        {
            std::error_code ignored;
            std::filesystem::remove_all(path, ignored);
        }
        std::filesystem::path path;
    };

    std::string const png("\x89PNG\r\n\x1a\n\0\0\0\rIHDR", 16);
    std::string const jpeg("\xff\xd8\xff\xe0\0\x10JFIF\0", 11);

    std::string preview_url(std::uint64_t const id)
    {
        return fmt::format("https://images.steamusercontent.com/ugc/{}/ABCDEF/", id);
    }

    std::string sized(std::uint64_t const id)
    {
        return Thumbnails::SizedPreviewUrl(preview_url(id), Thumbnails::ThumbnailService::image_pixels);
    }

    std::string response_with_previews(std::vector<std::uint64_t> const &ids)
    {
        std::string items;
        for (auto const id : ids)
            items += fmt::format(R"({}{{"publishedfileid":"{}","result":1,"preview_url":"{}","title":"mod {}"}})",
                                 items.empty() ? "" : ",", id, preview_url(id), id);
        return fmt::format(R"({{"response":{{"result":1,"publishedfiledetails":[{}]}}}})", items);
    }

    std::string request_body(std::vector<std::uint64_t> const &ids)
    {
        return WorkshopDetails::BuildRequestBodies(ids).at(0);
    }

    struct Results
    {
        std::map<std::uint64_t, std::optional<std::filesystem::path>> images;
        int calls = 0;

        Thumbnails::Callback callback()
        {
            return [this](std::uint64_t const id, std::optional<std::filesystem::path> image)
            {
                ++calls;
                images[id] = std::move(image);
            };
        }
    };
}

TEST_CASE("SizedPreviewUrl")
{
    using Thumbnails::SizedPreviewUrl;
    CHECK_EQ(SizedPreviewUrl("https://images.steamusercontent.com/ugc/1/ABC/", 512),
             "https://images.steamusercontent.com/ugc/1/ABC/?imw=512&imh=512&ima=fit&impolicy=Letterbox"
             "&imcolor=%23000000&letterbox=false");
    CHECK_EQ(SizedPreviewUrl("https://images.steamusercontent.com/ugc/1/ABC/?x=1", 256),
             "https://images.steamusercontent.com/ugc/1/ABC/?x=1&imw=256&imh=256&ima=fit&impolicy=Letterbox"
             "&imcolor=%23000000&letterbox=false");
    // Other hosts are left alone
    CHECK_EQ(SizedPreviewUrl("https://example.com/image.png", 512), "https://example.com/image.png");
    CHECK_EQ(SizedPreviewUrl("https://images.steamusercontent.com.evil.example/x", 512),
             "https://images.steamusercontent.com.evil.example/x");
    CHECK_EQ(SizedPreviewUrl("", 512), "");
}

TEST_CASE("Resolve_FreshCache_NoNetwork")
{
    TemporaryDirectory directory;
    ThumbnailCache cache(directory.path);
    auto const stored = cache.Store(42, png);
    REQUIRE(stored.has_value());

    HttpClientMock http; // any call fails the test
    Thumbnails::ThumbnailService service(http, cache);
    Results results;
    service.Resolve({42}, results.callback());

    CHECK_EQ(results.calls, 1);
    CHECK_EQ(results.images[42], stored);
}

TEST_CASE("Resolve_Missing_FetchesStoresAndReports")
{
    TemporaryDirectory directory;
    ThumbnailCache cache(directory.path);
    HttpClientMock http;
    REQUIRE_CALL(http, PostForm(WorkshopDetails::endpoint, request_body({42, 43})))
    .RETURN(response_with_previews({42, 43}));
    REQUIRE_CALL(http, Get(sized(42))).RETURN(png);
    REQUIRE_CALL(http, Get(sized(43))).RETURN(jpeg);

    Thumbnails::ThumbnailService service(http, cache);
    Results results;
    service.Resolve({42, 43}, results.callback());

    CHECK_EQ(results.calls, 2);
    CHECK_EQ(results.images[42], directory.path / "thumbnails" / "42.png");
    CHECK_EQ(results.images[43], directory.path / "thumbnails" / "43.jpg");
    REQUIRE(cache.Lookup(42).has_value());
    CHECK(cache.Lookup(42)->fresh);
}

TEST_CASE("Resolve_OnlyMissingIdsAreRequested")
{
    TemporaryDirectory directory;
    ThumbnailCache cache(directory.path);
    REQUIRE(cache.Store(42, png).has_value());

    HttpClientMock http;
    REQUIRE_CALL(http, PostForm(WorkshopDetails::endpoint, request_body({43}))).RETURN(response_with_previews({43}));
    REQUIRE_CALL(http, Get(sized(43))).RETURN(png);

    Thumbnails::ThumbnailService service(http, cache);
    Results results;
    service.Resolve({42, 43}, results.callback());
    CHECK_EQ(results.calls, 2);
    CHECK(results.images[42].has_value());
    CHECK(results.images[43].has_value());
}

TEST_CASE("Resolve_Stale_RefreshedWhenOnline")
{
    TemporaryDirectory directory;
    REQUIRE(ThumbnailCache(directory.path).Store(42, png).has_value());
    ThumbnailCache later(directory.path, 24h * 7, []()
    {
        return std::chrono::system_clock::now() + 24h * 8;
    });

    HttpClientMock http;
    REQUIRE_CALL(http, PostForm(WorkshopDetails::endpoint, request_body({42}))).RETURN(response_with_previews({42}));
    REQUIRE_CALL(http, Get(sized(42))).RETURN(jpeg);

    Thumbnails::ThumbnailService service(http, later);
    Results results;
    service.Resolve({42}, results.callback());
    CHECK_EQ(results.images[42], directory.path / "thumbnails" / "42.jpg");
}

TEST_CASE("Resolve_Offline_FallsBackToStaleOrNothing")
{
    TemporaryDirectory directory;
    auto const stale = ThumbnailCache(directory.path).Store(42, png);
    REQUIRE(stale.has_value());
    ThumbnailCache later(directory.path, 24h * 7, []()
    {
        return std::chrono::system_clock::now() + 24h * 8;
    });

    HttpClientMock http;
    REQUIRE_CALL(http, PostForm(WorkshopDetails::endpoint, request_body({42, 43}))).RETURN(std::nullopt);

    Thumbnails::ThumbnailService service(http, later);
    Results results;
    service.Resolve({42, 43}, results.callback());
    CHECK_EQ(results.calls, 2);
    CHECK_EQ(results.images[42], stale);
    CHECK_FALSE(results.images[43].has_value());
}

TEST_CASE("Resolve_DownloadFailures_ReportNothing")
{
    TemporaryDirectory directory;
    ThumbnailCache cache(directory.path);
    HttpClientMock http;
    REQUIRE_CALL(http, PostForm(WorkshopDetails::endpoint, request_body({1, 2, 3})))
    .RETURN(response_with_previews({1, 2})); // 3 has no preview image
    REQUIRE_CALL(http, Get(sized(1))).RETURN(std::nullopt);
    REQUIRE_CALL(http, Get(sized(2))).RETURN(std::string("<html>not an image</html>"));

    Thumbnails::ThumbnailService service(http, cache);
    Results results;
    service.Resolve({1, 2, 3}, results.callback());
    CHECK_EQ(results.calls, 3);
    CHECK_FALSE(results.images[1].has_value());
    CHECK_FALSE(results.images[2].has_value());
    CHECK_FALSE(results.images[3].has_value());
    CHECK_FALSE(cache.Lookup(2).has_value());
}

TEST_CASE("Resolve_BatchesOf100AndDeduplicates")
{
    TemporaryDirectory directory;
    ThumbnailCache cache(directory.path);
    std::vector<std::uint64_t> ids;
    for (std::uint64_t id = 1; id <= 150; ++id)
        ids.push_back(id);
    auto requested = ids;
    requested.push_back(5); // duplicate

    std::vector<std::uint64_t> const first(ids.begin(), ids.begin() + 100);
    std::vector<std::uint64_t> const second(ids.begin() + 100, ids.end());
    HttpClientMock http;
    REQUIRE_CALL(http, PostForm(WorkshopDetails::endpoint, request_body(first))).RETURN(response_with_previews({}));
    REQUIRE_CALL(http, PostForm(WorkshopDetails::endpoint, request_body(second))).RETURN(response_with_previews({}));

    Thumbnails::ThumbnailService service(http, cache);
    Results results;
    service.Resolve(requested, results.callback());
    CHECK_EQ(results.calls, 150);
}

TEST_CASE("Resolve_Stopped_NoFurtherWork")
{
    TemporaryDirectory directory;
    ThumbnailCache cache(directory.path);
    std::atomic<bool> stop = false;

    HttpClientMock http;
    REQUIRE_CALL(http, PostForm(WorkshopDetails::endpoint, request_body({1, 2}))).RETURN(response_with_previews({1, 2}));
    REQUIRE_CALL(http, Get(sized(1))).LR_SIDE_EFFECT(stop = true).RETURN(png);

    Thumbnails::ThumbnailService service(http, cache);
    Results results;
    service.Resolve({1, 2}, results.callback(), &stop);
    CHECK_EQ(results.calls, 0);
}

TEST_CASE("Resolve_AlreadyStopped_NoCallbacks")
{
    TemporaryDirectory directory;
    ThumbnailCache cache(directory.path);
    REQUIRE(cache.Store(1, png).has_value());
    std::atomic<bool> const stop = true;

    HttpClientMock http;
    Thumbnails::ThumbnailService service(http, cache);
    Results results;
    service.Resolve({1, 2}, results.callback(), &stop);
    CHECK_EQ(results.calls, 0);
}

namespace
{
    // Thread-safe fake for the worker tests: every id has a preview, downloads can be held open
    class FakeHttp : public Http::IHttpClient
    {
        public:
            explicit FakeHttp(bool hold_downloads = false) : hold_downloads_(hold_downloads) {}

            std::optional<std::string> PostForm(std::string const &, std::string const &body) override
            {
                std::vector<std::uint64_t> ids;
                for (size_t position = body.find("%5D="); position != std::string::npos; position = body.find("%5D=", position + 1))
                    ids.push_back(std::stoull(body.substr(position + 4)));
                return response_with_previews(ids);
            }

            std::optional<std::string> Get(std::string const &) override
            {
                std::unique_lock lock(mutex_);
                ++downloads_started_;
                changed_.notify_all();
                changed_.wait(lock, [this]()
                {
                    return !hold_downloads_ || released_;
                });
                return png;
            }

            bool WaitForDownloadStarted()
            {
                std::unique_lock lock(mutex_);
                return changed_.wait_for(lock, 10s, [this]()
                {
                    return downloads_started_ > 0;
                });
            }

            void Release()
            {
                std::lock_guard lock(mutex_);
                released_ = true;
                changed_.notify_all();
            }

        private:
            bool hold_downloads_;
            bool released_ = false;
            int downloads_started_ = 0;
            std::mutex mutex_;
            std::condition_variable changed_;
    };

    struct ThreadSafeResults
    {
        std::mutex mutex;
        std::condition_variable changed;
        std::map<std::uint64_t, std::thread::id> threads;

        Thumbnails::Callback callback()
        {
            return [this](std::uint64_t const id, std::optional<std::filesystem::path> image)
            {
                std::lock_guard lock(mutex);
                if (image)
                    threads[id] = std::this_thread::get_id();
                changed.notify_all();
            };
        }

        bool wait_for(size_t const count)
        {
            std::unique_lock lock(mutex);
            return changed.wait_for(lock, 10s, [&]()
            {
                return threads.size() >= count;
            });
        }
    };
}

TEST_CASE("Worker_ResolvesOnBackgroundThread")
{
    TemporaryDirectory directory;
    ThreadSafeResults results;
    {
        Thumbnails::ThumbnailWorker worker(std::make_unique<FakeHttp>(), ThumbnailCache(directory.path));
        worker.Request({1, 2}, results.callback());
        worker.Request({3}, results.callback());
        REQUIRE(results.wait_for(3));
    }
    for (auto const &[id, thread] : results.threads)
    {
        CAPTURE(id);
        CHECK_NE(thread, std::this_thread::get_id());
    }
    CHECK(std::filesystem::exists(directory.path / "thumbnails" / "3.png"));
}

TEST_CASE("Worker_DestructionCancelsPendingWork")
{
    TemporaryDirectory directory;
    ThreadSafeResults results;
    auto http = std::make_unique<FakeHttp>(true);
    auto &fake = *http;
    std::thread releaser;
    {
        Thumbnails::ThumbnailWorker worker(std::move(http), ThumbnailCache(directory.path));
        worker.Request({1, 2}, results.callback());
        worker.Request({3}, results.callback());
        REQUIRE(fake.WaitForDownloadStarted());
        // Let the blocked download finish only while the worker is being destroyed
        releaser = std::thread([&fake]()
        {
            std::this_thread::sleep_for(50ms);
            fake.Release();
        });
    }
    releaser.join();
    std::lock_guard lock(results.mutex);
    CHECK(results.threads.empty());
}
