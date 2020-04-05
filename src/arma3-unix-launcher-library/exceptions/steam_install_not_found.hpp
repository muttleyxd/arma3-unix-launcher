#pragma once

#include <exception>

class SteamInstallNotFoundException : public std::exception
{
    public:
        char const *what() const noexcept override;
};
