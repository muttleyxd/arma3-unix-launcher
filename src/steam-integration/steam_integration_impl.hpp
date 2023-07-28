#pragma once

#include "steam_integration_stub.hpp"
#include <string>

namespace Steam
{
    class SteamIntegration : public IntegrationStub
    {
        public:
            SteamIntegration(char const *const app_id);
            ~SteamIntegration() override;

            bool reinitialize() override;

            bool is_initialized() const override;
            bool is_running() const override;

            Structs::DownloadInfo get_download_info(std::uint64_t const id) const override;
            std::string get_item_title(std::uint64_t const id) override;
            Structs::SubscriptionInfo get_mod_status(std::uint64_t const id) const override;
            std::vector<std::uint64_t> get_subscribed_items() const override;
            bool subscribe(std::uint64_t const id) const override;

            void resume_downloads() const override;
            void suspend_downloads() const override;

            void poll_events() const override;

            void set_item_downloaded_callback(std::function<void(Structs::ItemDownloadedInfo)> &&callback) override;

        private:
            std::unique_ptr<CallbackCatcher> callback_catcher;
            std::map<std::uint64_t, std::string> title_cache;
            std::mutex title_cache_access;
            std::string app_id;
    };
}
