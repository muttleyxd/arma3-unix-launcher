#include "thumbnail_service.hpp"

#include <algorithm>
#include <map>
#include <set>
#include <string_view>

#include <spdlog/spdlog.h>

#include "workshop_details.hpp"

namespace
{
    constexpr std::string_view steam_image_cdn = "https://images.steamusercontent.com/";
}

namespace Thumbnails
{
    std::string SizedPreviewUrl(std::string const &preview_url, int const pixels)
    {
        if (preview_url.rfind(steam_image_cdn, 0) != 0)
            return preview_url;
        char const separator = preview_url.find('?') == std::string::npos ? '?' : '&';
        return fmt::format("{}{}imw={}&imh={}&ima=fit&impolicy=Letterbox&imcolor=%23000000&letterbox=false",
                           preview_url, separator, pixels, pixels);
    }

    ThumbnailService::ThumbnailService(Http::IHttpClient &http, ThumbnailCache &cache)
        : http_(http)
        , cache_(cache)
    {
    }

    void ThumbnailService::Resolve(std::vector<std::uint64_t> const &ids, Callback const &callback,
                                   std::atomic<bool> const *stop)
    {
        auto const stopped = [stop]()
        {
            return stop && stop->load();
        };

        std::set<std::uint64_t> seen;
        std::vector<std::uint64_t> to_fetch;
        std::map<std::uint64_t, std::filesystem::path> stale;
        for (auto const id : ids)
        {
            if (!seen.insert(id).second)
                continue;
            if (stopped())
                return;

            auto const entry = cache_.Lookup(id);
            if (entry && entry->fresh)
            {
                callback(id, entry->path);
                continue;
            }
            if (entry)
                stale.emplace(id, entry->path);
            to_fetch.push_back(id);
        }

        auto const stale_image = [&stale](std::uint64_t const id) -> std::optional<std::filesystem::path>
        {
            auto const it = stale.find(id);
            if (it == stale.end())
                return std::nullopt;
            return it->second;
        };

        for (size_t batch_start = 0; batch_start < to_fetch.size(); batch_start += WorkshopDetails::max_ids_per_request)
        {
            if (stopped())
                return;
            size_t const batch_end = std::min(to_fetch.size(), batch_start + WorkshopDetails::max_ids_per_request);
            std::vector<std::uint64_t> const batch(to_fetch.begin() + batch_start, to_fetch.begin() + batch_end);

            std::map<std::uint64_t, std::string> preview_urls;
            auto const response = http_.PostForm(WorkshopDetails::endpoint, WorkshopDetails::BuildRequestBodies(batch).at(0));
            if (response)
                for (auto &item : WorkshopDetails::ParseResponse(*response))
                    preview_urls.emplace(item.id, std::move(item.preview_url));
            else
                spdlog::info("Workshop details request failed, using cached thumbnails where available");

            for (auto const id : batch)
            {
                if (stopped())
                    return;
                std::optional<std::filesystem::path> image;
                if (auto const url = preview_urls.find(id); url != preview_urls.end())
                    if (auto const bytes = http_.Get(SizedPreviewUrl(url->second, image_pixels)))
                        image = cache_.Store(id, *bytes);
                if (stopped())
                    return;
                callback(id, image ? image : stale_image(id));
            }
        }
    }

    ThumbnailWorker::ThumbnailWorker(std::unique_ptr<Http::IHttpClient> http, ThumbnailCache cache)
        : http_(std::move(http))
        , cache_(std::move(cache))
        , service_(*http_, cache_)
    {
        thread_ = std::thread(&ThumbnailWorker::Run, this);
    }

    ThumbnailWorker::~ThumbnailWorker()
    {
        {
            std::lock_guard lock(mutex_);
            stop_ = true;
        }
        wake_.notify_all();
        thread_.join();
    }

    void ThumbnailWorker::Request(std::vector<std::uint64_t> ids, Callback callback)
    {
        {
            std::lock_guard lock(mutex_);
            queue_.emplace_back(std::move(ids), std::move(callback));
        }
        wake_.notify_one();
    }

    void ThumbnailWorker::Run()
    {
        while (true)
        {
            std::unique_lock lock(mutex_);
            wake_.wait(lock, [this]()
            {
                return stop_ || !queue_.empty();
            });
            if (stop_)
                return;
            auto [ids, callback] = std::move(queue_.front());
            queue_.pop_front();
            lock.unlock();

            service_.Resolve(ids, callback, &stop_);
        }
    }
}
