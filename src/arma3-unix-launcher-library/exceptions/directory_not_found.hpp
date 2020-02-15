#pragma once

#include <exception>
#include <string>

class DirectoryNotFoundException : public std::exception
{
    public:
        explicit DirectoryNotFoundException(std::string const &path);

        const char *what() const noexcept override;

    private:
        std::string message;
};
