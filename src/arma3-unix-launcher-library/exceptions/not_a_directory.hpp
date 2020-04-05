#pragma once

#include <exception>
#include <string>

class NotADirectoryException : public std::exception
{
    public:
        explicit NotADirectoryException(std::string const &path);

        char const *what() const noexcept override;

    private:
        std::string message;
};
