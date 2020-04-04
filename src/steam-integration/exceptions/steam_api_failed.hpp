#pragma once

#include <exception>
#include <string>

class SteamApiFailedException : public std::exception
{
    public:
        explicit SteamApiFailedException(std::string const &path);

        char const *what() const noexcept override;

    private:
        std::string message;
};
