#ifndef EXCEPTIONS_STEAM_WORKSHOP_DIRECTORY_NOT_FOUND_HPP_
#define EXCEPTIONS_STEAM_WORKSHOP_DIRECTORY_NOT_FOUND_HPP_

#include <exception>
#include <string>

class SteamWorkshopDirectoryNotFoundException : public std::exception
{
    public:
        SteamWorkshopDirectoryNotFoundException(std::string const &appid);

        const char *what() const noexcept override;

    private:
        std::string msg_;
};

#endif
