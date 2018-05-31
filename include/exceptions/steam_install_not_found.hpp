#ifndef __EXCEPTIONS_STEAM_INSTALL_NOT_FOUND_HPP
#define __EXCEPTIONS_STEAM_INSTALL_NOT_FOUND_HPP

#include <exception>

class SteamInstallNotFoundException : public std::exception
{
    public:
        const char *what() const throw()
        {
            return "Steam installation not found";
        }
};

#endif
