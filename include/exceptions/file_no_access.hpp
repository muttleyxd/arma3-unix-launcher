#ifndef EXCEPTIONS_FILE_NO_ACCESS_HPP_
#define EXCEPTIONS_FILE_NO_ACCESS_HPP_

#include <exception>
#include <string>

class FileNoAccessException : public std::exception
{
    public:
        FileNoAccessException(const std::string &path);

        const char *what() const noexcept override;

    private:
        std::string msg_;
};

#endif
