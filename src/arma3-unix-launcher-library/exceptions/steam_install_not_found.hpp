#pragma once

#include <exception>

class SteamInstallNotFoundException : public std::exception
{
    public:
        const char *what() const noexcept override;
};
