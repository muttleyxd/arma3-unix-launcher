#pragma once

#include <exception>
#include <string>

class NotASymlinkException : public std::exception
{
    public:
        explicit NotASymlinkException(std::string const &path);

        char const *what() const noexcept override;

    private:
        std::string message;
};
