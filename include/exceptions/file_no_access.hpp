#ifndef __EXCEPTIONS_FILE_NO_ACCESS_HPP
#define __EXCEPTIONS_FILE_NO_ACCESS_HPP

#include <exception>

class FileNoAccessException : public std::exception
{
    public:
        FileNoAccessException(const std::string &path)
        {
            msg_ = "Cannot access file: " + path;
        }

        const char *what() const noexcept override
        {
            return msg_.c_str();
        }

    private:
        std::string msg_;
};

#endif
