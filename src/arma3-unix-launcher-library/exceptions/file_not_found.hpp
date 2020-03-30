#pragma once

#include <exception>
#include <string>

class FileNotFoundException : public std::exception
{
    public:
        explicit FileNotFoundException(std::string const &path);

        char const *what() const noexcept override;

    private:
        std::string message;
};
