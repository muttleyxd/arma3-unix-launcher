#pragma once

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <mutex>

class CallbackCatcher;

namespace Steam
{
    bool integration_support_enabled();

    namespace Structs
    {
        struct DownloadInfo
        {
            std::uint64_t id;
            std::uint64_t bytes_downloaded;
            std::uint64_t bytes_total;
        };

        struct ItemDownloadedInfo
        {
            std::uint64_t app_id;
            std::uint64_t workshop_id;
        };

        struct SubscriptionInfo
        {
            std::uint64_t id;
            bool downloading;
            bool download_pending;
            bool installed;
            bool needs_update;
            bool subscribed;
        };
    }

    class Integration
    {
        public:
            Integration(char const* const app_id);
            ~Integration();

            bool reinitialize();

            bool is_initialized() const;
            bool is_running() const;

            Structs::DownloadInfo get_download_info(std::uint64_t const id) const;
            std::string get_item_title(std::uint64_t const id);
            Structs::SubscriptionInfo get_mod_status(std::uint64_t const id) const;
            std::vector<std::uint64_t> get_subscribed_items() const;
            bool subscribe(std::uint64_t const id) const;

            void resume_downloads() const;
            void suspend_downloads() const;

            void poll_events() const;

            void set_item_downloaded_callback(std::function<void(Structs::ItemDownloadedInfo)> &&callback);

        private:
            std::unique_ptr<CallbackCatcher> callback_catcher;
            std::map<std::uint64_t, std::string> title_cache;
            std::mutex title_cache_access;
            std::string app_id;
    };
}
