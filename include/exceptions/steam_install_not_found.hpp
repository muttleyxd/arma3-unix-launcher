#ifndef EXCEPTIONS_STEAM_INSTALL_NOT_FOUND_HPP_
#define EXCEPTIONS_STEAM_INSTALL_NOT_FOUND_HPP_

#include <exception>

class SteamInstallNotFoundException : public std::exception
{
    public:
        const char *what() const noexcept override;
};

#endif
