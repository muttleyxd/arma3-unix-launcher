#pragma once

#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include <trompeloeil.hpp>

#include <filesystem>
#include <string>
#include <vector>

/*
#include <filesystem>
#include <string>
#include <vector>

class SteamUtils
{
    public:
        SteamUtils(std::vector<std::filesystem::path> search_paths = {"$HOME/.local/share/Steam", "$HOME/.steam/steam", "$HOME/Library/Application Support/Steam"});
d
        std::filesystem::path const &GetSteamPath() const noexcept;
        std::vector<std::filesystem::path> GetInstallPaths() const;
        std::filesystem::path GetGamePathFromInstallPath(std::filesystem::path const &install_path,
                std::string const &appid) const;
        std::filesystem::path GetWorkshopPath(std::filesystem::path const &install_path, std::string const &appid) const;
        std::uint64_t GetCompatibilityToolForAppId(std::uint64_t const app_id);
        std::filesystem::path GetInstallPathFromGamePath(std::filesystem::path const &game_path);

    private:
        std::filesystem::path steam_path_;
        std::filesystem::path config_path_ = "config/config.vdf";
};
*/

class SteamUtils;

class SteamUtilsMock
{
    public:
        SteamUtilsMock()
        {
            instance = this;
        }
        ~SteamUtilsMock()
        {
            instance = nullptr;
        }
        static inline SteamUtilsMock *instance;

        MAKE_MOCK2(Constructor, void(std::vector<std::filesystem::path>, SteamUtils &));

        MAKE_MOCK1(GetSteamPath, std::filesystem::path const & (SteamUtils const &));
        MAKE_MOCK1(GetInstallPaths, std::vector<std::filesystem::path>(SteamUtils const &));
        MAKE_MOCK3(GetGamePathFromInstallPath, std::filesystem::path(std::filesystem::path const &,
                   std::string const &, SteamUtils const &));
        MAKE_MOCK3(GetWorkshopPath, std::filesystem::path(std::filesystem::path const &, std::string const &,
                   SteamUtils const &));
        MAKE_MOCK2(GetCompatibilityToolForAppId, std::uint64_t(std::uint64_t const, SteamUtils const &));
        MAKE_MOCK2(GetInstallPathFromGamePath, std::filesystem::path(std::filesystem::path const &, SteamUtils const &));
};

SteamUtils::SteamUtils(std::vector<std::filesystem::path> search_paths)
{
    SteamUtilsMock::instance->Constructor(search_paths, *this);
}

std::filesystem::path const &SteamUtils::GetSteamPath() const noexcept
{
    return SteamUtilsMock::instance->GetSteamPath(*this);
}

std::vector<std::filesystem::path> SteamUtils::GetInstallPaths() const
{
    return SteamUtilsMock::instance->GetInstallPaths(*this);
}

std::filesystem::path SteamUtils::GetGamePathFromInstallPath(std::filesystem::path const &install_path,
        std::string const &appid) const
{
    return SteamUtilsMock::instance->GetGamePathFromInstallPath(install_path, appid, *this);
}

std::filesystem::path SteamUtils::GetWorkshopPath(std::filesystem::path const &install_path,
        std::string const &appid) const
{
    return SteamUtilsMock::instance->GetWorkshopPath(install_path, appid, *this);
}

std::uint64_t SteamUtils::GetCompatibilityToolForAppId(std::uint64_t const app_id)
{
    return SteamUtilsMock::instance->GetCompatibilityToolForAppId(app_id, *this);
}

std::filesystem::path SteamUtils::GetInstallPathFromGamePath(std::filesystem::path const &game_path)
{
    return SteamUtilsMock::instance->GetInstallPathFromGamePath(game_path, *this);
}
