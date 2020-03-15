#pragma once

#include <exception>
#include <string>

class SyntaxErrorException : public std::exception
{
    public:
        explicit SyntaxErrorException(std::string const &error);

        const char *what() const noexcept override;

    private:
        std::string message;
};
