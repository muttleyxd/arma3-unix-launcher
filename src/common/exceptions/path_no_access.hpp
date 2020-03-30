#pragma once

#include <exception>
#include <string>

class PathNoAccessException : public std::exception
{
    public:
        explicit PathNoAccessException(std::string const &path);

        const char *what() const noexcept override;

    private:
        std::string message;
};
