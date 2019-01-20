#ifndef EXCEPTIONS_DIRECTORY_NO_ACCESS_HPP_
#define EXCEPTIONS_DIRECTORY_NO_ACCESS_HPP_

#include <exception>
#include <string>

class DirectoryNoAccessException : public std::exception
{
    public:
        DirectoryNoAccessException(std::string const &path);

        const char *what() const noexcept override;

    private:
        std::string msg_;
};

#endif
