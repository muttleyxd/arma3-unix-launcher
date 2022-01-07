#pragma once

#include <exception>
#include <string>

class PresetLoadingFailedException : public std::exception
{
    public:
        explicit PresetLoadingFailedException(std::string const &error);

        char const *what() const noexcept override;

    private:
        std::string message;
};
