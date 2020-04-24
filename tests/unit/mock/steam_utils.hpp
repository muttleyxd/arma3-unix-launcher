#pragma once

#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include <trompeloeil.hpp>

#include <filesystem>
#include <string>
#include <vector>

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

        MAKE_MOCK2(Constructor, void(std::vector<std::filesystem::path> const &, SteamUtils &));

        MAKE_MOCK1(GetSteamPath, std::filesystem::path const & (SteamUtils const &));
        MAKE_MOCK1(GetInstallPaths, std::vector<std::filesystem::path>(SteamUtils const &));
        MAKE_MOCK3(GetGamePathFromInstallPath, std::filesystem::path(std::filesystem::path const &,
                   std::string_view const, SteamUtils const &));
        MAKE_MOCK3(GetWorkshopPath, std::filesystem::path(std::filesystem::path const &, std::string const &,
                   SteamUtils const &));
        MAKE_MOCK2(GetCompatibilityToolForAppId, (std::pair<std::filesystem::path, std::string>(std::uint64_t const,
                   SteamUtils const &)));
        MAKE_MOCK2(GetInstallPathFromGamePath, std::filesystem::path(std::filesystem::path const &, SteamUtils const &));
};

SteamUtils::SteamUtils(std::vector<std::filesystem::path> const &search_paths)
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
        std::string_view const appid) const
{
    return SteamUtilsMock::instance->GetGamePathFromInstallPath(install_path, appid, *this);
}

std::filesystem::path SteamUtils::GetWorkshopPath(std::filesystem::path const &install_path,
        std::string const &appid) const
{
    return SteamUtilsMock::instance->GetWorkshopPath(install_path, appid, *this);
}

std::pair<std::filesystem::path, std::string> SteamUtils::GetCompatibilityToolForAppId(std::uint64_t const app_id)
{
    return SteamUtilsMock::instance->GetCompatibilityToolForAppId(app_id, *this);
}

std::filesystem::path SteamUtils::GetInstallPathFromGamePath(std::filesystem::path const &game_path)
{
    return SteamUtilsMock::instance->GetInstallPathFromGamePath(game_path, *this);
}
