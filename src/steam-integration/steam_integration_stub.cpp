#include "steam_integration_stub.hpp"

class CallbackCatcher
{
};

namespace Steam
{
    IntegrationStub::IntegrationStub(char const *const)
    {
    }

    bool IntegrationStub::reinitialize()
    {
        return false;
    }

    bool IntegrationStub::is_initialized() const
    {
        return false;
    }

    bool IntegrationStub::is_running() const
    {
        return false;
    }

    Structs::DownloadInfo IntegrationStub::get_download_info(uint64_t const) const
    {
        return {};
    }

    std::string IntegrationStub::get_item_title(std::uint64_t const)
    {
        return "";
    }

    Structs::SubscriptionInfo IntegrationStub::get_mod_status(std::uint64_t const) const
    {
        return {};
    }

    std::vector<std::uint64_t> IntegrationStub::get_subscribed_items() const
    {
        return {};
    }

    bool IntegrationStub::subscribe(std::uint64_t const) const
    {
        return false;
    }

    void IntegrationStub::resume_downloads() const
    {
    }

    void IntegrationStub::suspend_downloads() const
    {
    }

    void IntegrationStub::poll_events() const
    {
    }

    void IntegrationStub::set_item_downloaded_callback(std::function<void (Structs::ItemDownloadedInfo)> &&)
    {
    }
}
