#pragma once

#include <exception>

class SteamApiNotInitializedException : public std::exception
{
    public:
        char const *what() const noexcept override;
};
