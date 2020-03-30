#pragma once

#include <exception>
#include <string>

class PathNoAccessException : public std::exception
{
    public:
        explicit PathNoAccessException(std::string const &path);

        char const *what() const noexcept override;

    private:
        std::string message;
};
