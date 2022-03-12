#include "steam_integration_impl.hpp"

#include <atomic>
#include <filesystem>
#include <fstream>

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <steam/steam_api.h>

#include "filesystem_utils.hpp"
#include "std_utils.hpp"

#include "exceptions/steam_api_failed.hpp"
#include "exceptions/steam_api_not_initialized.hpp"

namespace fs = FilesystemUtils;

namespace
{
    static std::atomic_bool initialized = false;
    static std::atomic_int steam_integration_counter = 0;

    bool has_flag(uint64_t status, uint64_t flag)
    {
        return (status & flag) == flag;
    }

    bool is_steam_flatpak()
    {
        try
        {
            std::filesystem::path const home_directory = getenv("HOME");
            auto const steam_flatpak_path = home_directory / ".var/app/com.valvesoftware.Steam";
            auto const path = home_directory / ".steam/steam/linux64/steamclient.so";

            return fs::Exists(steam_flatpak_path) || (fs::Exists(path)
                    && StdUtils::Contains(fs::RealPath(path), "com.valvesoftware.Steam"));
        }
        catch (std::exception const &e)
        {
            spdlog::warn("is_steam_flatpak() exception, disabling SteamAPI SteamIntegration");
            return true;
        }
    }

    bool initialize_steam_api(std::string const &app_id)
    {
        if (initialized)
            return true;

        if (is_steam_flatpak())
        {
            spdlog::warn("Steam initialization failed - Flatpak detected!");
            return false;
        }

        auto old_path = fs::CurrentPath();
        std::filesystem::path tmp_path = fmt::format("{}/arma3-unix-launcher.{}", fs::TempDirectoryPath().string(), getpid());
        fs::CreateDirectories(tmp_path);
        fs::CurrentPath(tmp_path);
        StdUtils::FileWriteAllText(tmp_path / "steam_appid.txt", app_id);

        initialized = SteamAPI_Init();

        fs::RemoveAll(tmp_path);
        fs::CurrentPath(old_path);

        return initialized;
    }

    void deinitialize_steam_api()
    {
        if (steam_integration_counter == 0 && initialized)
        {
            SteamAPI_Shutdown();
            initialized = false;
        }
    }
}

class CallbackCatcher
{
    public:
        std::function<void(Steam::Structs::ItemDownloadedInfo)> item_installed_callback;
        std::function<void(std::pair<std::uint64_t, std::string>)> workshop_name_callback;

        CCallResult<CallbackCatcher, SteamUGCQueryCompleted_t> on_query_completed;
        void OnQueryCompleted(SteamUGCQueryCompleted_t *info, bool failure);

    private:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof" // Can't do much about it
        STEAM_CALLBACK(CallbackCatcher, OnItemInstalled, ItemInstalled_t);
#pragma GCC diagnostic pop
};


void CallbackCatcher::OnItemInstalled(ItemInstalled_t *info)
{
    if (info != nullptr && item_installed_callback)
        item_installed_callback({info->m_unAppID, info->m_nPublishedFileId});
}

void CallbackCatcher::OnQueryCompleted(SteamUGCQueryCompleted_t *info, bool failure)
{
    auto steam_ugc = SteamUGC();

    if (!info)
        return;

    if (workshop_name_callback && !failure && info->m_eResult == EResult::k_EResultOK)
    {
        for (size_t i = 0; i < info->m_unNumResultsReturned; ++i)
        {
            SteamUGCDetails_t item_details{};
            if (steam_ugc->GetQueryUGCResult(info->m_handle, i, &item_details))
                workshop_name_callback({item_details.m_nPublishedFileId, item_details.m_rgchTitle});
        }
    }

    if (info->m_handle)
        steam_ugc->ReleaseQueryUGCRequest(info->m_handle);
}


namespace Steam
{
    SteamIntegration::SteamIntegration(char const *const app_id_) : IntegrationStub(app_id_), app_id(app_id_)
    {
        ++steam_integration_counter;
        SteamIntegration::reinitialize();
        callback_catcher = std::make_unique<CallbackCatcher>();
        callback_catcher->workshop_name_callback = [&](auto const & info)
        {
            std::scoped_lock lock(title_cache_access);
            title_cache[info.first] = info.second;
        };
    }

    SteamIntegration::~SteamIntegration()
    {
        --steam_integration_counter;
        deinitialize_steam_api();
    }

    bool SteamIntegration::reinitialize()
    {
        return initialize_steam_api(app_id);
    }

    bool SteamIntegration::is_initialized() const
    {
        return initialized;
    }

    bool SteamIntegration::is_running() const
    {
        return SteamAPI_IsSteamRunning();
    }

    Structs::DownloadInfo SteamIntegration::get_download_info(uint64_t const id) const
    {
        if (!is_initialized())
            throw SteamApiNotInitializedException();

        static_assert(sizeof(uint64) == sizeof(uint64_t), "Valve defined uint64 has different size than uint64_t");
        Structs::DownloadInfo ret{id, 0, 0};
        auto const status = SteamUGC()->GetItemDownloadInfo(id, reinterpret_cast<uint64 *>(&ret.bytes_downloaded),
                            reinterpret_cast<uint64 *>(&ret.bytes_total));
        if (!status)
            throw SteamApiFailedException("GetItemDownloadInfo failed");

        return ret;
    }

    std::string SteamIntegration::get_item_title(std::uint64_t const id)
    {
        if (!is_initialized())
            throw SteamApiNotInitializedException();

        {
            std::scoped_lock lock(title_cache_access);

            if (StdUtils::ContainsKey(title_cache, id))
                return title_cache[id];
        }

        uint64 internal_id = id;
        auto const handle = SteamUGC()->CreateQueryUGCDetailsRequest(&internal_id, 1);
        if (handle == k_UGCQueryHandleInvalid)
            spdlog::warn("exception in get_item_title({}): SteamUGC()->CreateQueryUGCDetailsRequest returned invalid handle", id);

        auto const result = SteamUGC()->SendQueryUGCRequest(handle);
        if (result == k_uAPICallInvalid)
        {
            SteamUGC()->ReleaseQueryUGCRequest(handle);
            spdlog::warn("exception in get_item_title({}): SteamUGC()->SendQueryUGCRequest returned invalid call", id);
        }
        else
            callback_catcher->on_query_completed.Set(result, callback_catcher.get(), &CallbackCatcher::OnQueryCompleted);

        return std::to_string(id);
    }

    Structs::SubscriptionInfo SteamIntegration::get_mod_status(std::uint64_t const id) const
    {
        if (!is_initialized())
            throw SteamApiNotInitializedException();

        auto const status = SteamUGC()->GetItemState(id);
        return
        {
            id,
            has_flag(status, k_EItemStateDownloading),
            has_flag(status, k_EItemStateDownloadPending),
            has_flag(status, k_EItemStateInstalled),
            has_flag(status, k_EItemStateNeedsUpdate),
            has_flag(status, k_EItemStateSubscribed)
        };
    }

    std::vector<uint64_t> SteamIntegration::get_subscribed_items() const
    {
        if (!is_initialized())
            throw SteamApiNotInitializedException();

        constexpr unsigned int max_items = 4096;
        std::vector<uint64_t> ret;

        auto steam_ugc = SteamUGC();

        auto const item_count = steam_ugc->GetNumSubscribedItems();
        if (item_count == 0)
            return ret;

        auto const array_size = item_count > max_items ? max_items : item_count;
        auto const array = std::make_unique<uint64[]>(array_size);

        auto const returned_item_count = steam_ugc->GetSubscribedItems(&array[0], array_size);
        if (returned_item_count == 0)
            throw SteamApiFailedException("SteamUGC()->GetSubscribedItems() <= 0");

        for (uint32 i = 0; i < returned_item_count; ++i)
            ret.push_back(array[i]);

        return ret;
    }

    bool SteamIntegration::subscribe(std::uint64_t const id) const
    {
        if (!is_initialized())
            throw SteamApiNotInitializedException();

        return SteamUGC()->SubscribeItem(id);
    }

    void SteamIntegration::resume_downloads() const
    {
        if (!is_initialized())
            throw SteamApiNotInitializedException();

        SteamUGC()->SuspendDownloads(false);
    }

    void SteamIntegration::suspend_downloads() const
    {
        if (!is_initialized())
            throw SteamApiNotInitializedException();

        SteamUGC()->SuspendDownloads(true);
    }

    void SteamIntegration::poll_events() const
    {
        if (!is_initialized())
            throw SteamApiNotInitializedException();

        SteamAPI_RunCallbacks();
    }

    void SteamIntegration::set_item_downloaded_callback(std::function<void (Structs::ItemDownloadedInfo)> &&callback)
    {
        callback_catcher->item_installed_callback = std::move(callback);
    }
}
