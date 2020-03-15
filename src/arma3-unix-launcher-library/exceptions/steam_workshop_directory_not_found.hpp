#pragma once

#include <exception>
#include <string>

class SteamWorkshopDirectoryNotFoundException : public std::exception
{
    public:
        explicit SteamWorkshopDirectoryNotFoundException(std::string const &appid);

        const char *what() const noexcept override;

    private:
        std::string message;
};
