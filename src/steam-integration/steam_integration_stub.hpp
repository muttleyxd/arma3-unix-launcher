#pragma once

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

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

    class IntegrationStub
    {
        public:
            IntegrationStub(char const *const app_id);
            virtual ~IntegrationStub() = default;

            virtual bool reinitialize();

            virtual bool is_initialized() const;
            virtual bool is_running() const;

            virtual Structs::DownloadInfo get_download_info(std::uint64_t const id) const;
            virtual std::string get_item_title(std::uint64_t const id);
            virtual Structs::SubscriptionInfo get_mod_status(std::uint64_t const id) const;
            virtual std::vector<std::uint64_t> get_subscribed_items() const;
            virtual bool subscribe(std::uint64_t const id) const;

            virtual void resume_downloads() const;
            virtual void suspend_downloads() const;

            virtual void poll_events() const;

            virtual void set_item_downloaded_callback(std::function<void(Structs::ItemDownloadedInfo)> &&callback);
    };
}
