#ifndef EXCEPTIONS_PATH_NO_ACCESS_HPP_
#define EXCEPTIONS_PATH_NO_ACCESS_HPP_

#include <exception>
#include <string>

class PathNoAccessException : public std::exception
{
    public:
        PathNoAccessException(std::string const &path);

        const char *what() const noexcept override;

    private:
        std::string msg_;
};

#endif
