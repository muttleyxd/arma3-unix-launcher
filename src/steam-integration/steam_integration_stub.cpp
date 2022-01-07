#include "steam_integration.hpp"

class CallbackCatcher
{
};

namespace Steam
{
    bool integration_support_enabled()
    {
        return false;
    }

    Integration::Integration(char const* const)
    {
    }

    Integration::~Integration()
    {
    }

    bool Integration::reinitialize()
    {
        return false;
    }

    bool Integration::is_initialized() const
    {
        return false;
    }

    bool Integration::is_running() const
    {
        return false;
    }

    Structs::DownloadInfo Integration::get_download_info(uint64_t const) const
    {
        return {};
    }

    std::string Integration::get_item_title(std::uint64_t const)
    {
        return "";
    }

    Structs::SubscriptionInfo Integration::get_mod_status(std::uint64_t const) const
    {
        return {};
    }

    std::vector<std::uint64_t> Integration::get_subscribed_items() const
    {
        return {};
    }

    bool Integration::subscribe(std::uint64_t const) const
    {
        return false;
    }

    void Integration::resume_downloads() const
    {
    }

    void Integration::suspend_downloads() const
    {
    }

    void Integration::poll_events() const
    {
    }

    void Integration::set_item_downloaded_callback(std::function<void (Structs::ItemDownloadedInfo)> &&)
    {
    }
}
