#pragma once

#include <exception>
#include <string>

class NotADirectoryException : public std::exception
{
    public:
        explicit NotADirectoryException(std::string const &path);

        const char *what() const noexcept override;

    private:
        std::string message;
};
