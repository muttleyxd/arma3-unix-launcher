#pragma once

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <filesystem>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include "http_client.hpp"
#include "thumbnail_cache.hpp"

namespace Thumbnails
{
    // Called once per requested id with the cached image, or std::nullopt if there is none
    using Callback = std::function<void(std::uint64_t, std::optional<std::filesystem::path>)>;

    // Preview image URL on Steam's image CDN resized to fit pixels x pixels; other URLs are unchanged
    std::string SizedPreviewUrl(std::string const &preview_url, int const pixels);

    class ThumbnailService
    {
        public:
            static constexpr int image_pixels = 512;

            ThumbnailService(Http::IHttpClient &http, ThumbnailCache &cache);

            // Fresh cached images are reported without network access. Missing and stale ones are looked
            // up through the Steam Web API in batches and downloaded; if that fails, a stale image is
            // still reported. Runs synchronously; returns early, without further callbacks, once *stop
            // becomes true.
            void Resolve(std::vector<std::uint64_t> const &ids, Callback const &callback,
                         std::atomic<bool> const *stop = nullptr);

        private:
            Http::IHttpClient &http_;
            ThumbnailCache &cache_;
    };

    // Runs ThumbnailService::Resolve on a background thread. Callbacks are invoked on that thread.
    // Destroying the worker cancels queued and running requests and joins the thread.
    class ThumbnailWorker
    {
        public:
            ThumbnailWorker(std::unique_ptr<Http::IHttpClient> http, ThumbnailCache cache);
            ~ThumbnailWorker();

            ThumbnailWorker(ThumbnailWorker const &) = delete;
            ThumbnailWorker &operator=(ThumbnailWorker const &) = delete;

            void Request(std::vector<std::uint64_t> ids, Callback callback);

        private:
            void Run();

            std::unique_ptr<Http::IHttpClient> http_;
            ThumbnailCache cache_;
            ThumbnailService service_;

            std::mutex mutex_;
            std::condition_variable wake_;
            std::deque<std::pair<std::vector<std::uint64_t>, Callback>> queue_;
            std::atomic<bool> stop_ = false;
            std::thread thread_;
    };
}
