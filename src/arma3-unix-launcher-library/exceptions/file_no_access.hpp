#pragma once

#include <exception>
#include <string>

class FileNoAccessException : public std::exception
{
    public:
        explicit FileNoAccessException(std::string const &path);

        char const *what() const noexcept override;

    private:
        std::string message;
};
