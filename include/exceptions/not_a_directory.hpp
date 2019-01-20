#ifndef EXCEPTIONS_NOT_A_DIRECTORY_HPP_
#define EXCEPTIONS_NOT_A_DIRECTORY_HPP_

#include <exception>
#include <string>

class NotADirectoryException : public std::exception
{
    public:
        NotADirectoryException(std::string const &path);

        const char *what() const noexcept override;

    private:
        std::string msg_;
};

#endif
