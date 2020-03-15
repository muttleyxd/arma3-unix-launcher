#pragma once

#include <exception>
#include <string>

class FileNotFoundException : public std::exception
{
    public:
        explicit FileNotFoundException(std::string const &path);

        const char *what() const noexcept override;

    private:
        std::string message;
};
