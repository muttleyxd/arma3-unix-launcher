#ifndef __EXCEPTIONS_STEAM_WORKSHOP_DIRECTORY_NOT_FOUND_HPP
#define __EXCEPTIONS_STEAM_WORKSHOP_DIRECTORY_NOT_FOUND_HPP

#include <exception>

class SteamWorkshopDirectoryNotFoundException : public std::exception
{
    public:
        SteamWorkshopDirectoryNotFoundException(std::string appid) : appid_(appid) {}

        const char *what() const throw()
        {
            return ("Steam Workshop directory not found for appid: " + appid_).c_str();
        }

    private:
        std::string appid_;
};

#endif
