#ifndef __EXCEPTIONS_STEAM_WORKSHOP_DIRECTORY_NOT_FOUND_HPP
#define __EXCEPTIONS_STEAM_WORKSHOP_DIRECTORY_NOT_FOUND_HPP

#include <exception>

class SteamWorkshopDirectoryNotFoundException : public std::exception
{
    public:
        SteamWorkshopDirectoryNotFoundException(std::string appid)
        {
            msg_ = "Steam Workshop directory not found for appid: " + appid;
        }

        const char *what() const noexcept override
        {
            return msg_.c_str();
        }

    private:
        std::string msg_;
};

#endif
