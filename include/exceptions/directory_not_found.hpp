#ifndef EXCEPTIONS_DIRECTORY_NOT_FOUND_HPP_
#define EXCEPTIONS_DIRECTORY_NOT_FOUND_HPP_

#include <exception>
#include <string>

class DirectoryNotFoundException : public std::exception
{
    public:
        DirectoryNotFoundException(std::string const &path);

        const char *what() const noexcept override;

    private:
        std::string msg_;
};

#endif
