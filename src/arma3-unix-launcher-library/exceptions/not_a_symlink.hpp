#pragma once

#include <exception>
#include <string>

class NotASymlinkException : public std::exception
{
    public:
        explicit NotASymlinkException(std::string const &path);

        const char *what() const noexcept override;

    private:
        std::string message;
};
