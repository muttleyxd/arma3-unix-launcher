#pragma once

#include <exception>
#include <string>

class DirectoryNoAccessException : public std::exception
{
    public:
        explicit DirectoryNoAccessException(std::string const &path);

        char const *what() const noexcept override;

    private:
        std::string message;
};
