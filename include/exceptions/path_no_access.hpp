#ifndef __EXCEPTIONS_PATH_NO_ACCESS_HPP
#define __EXCEPTIONS_PATH_NO_ACCESS_HPP

#include <exception>

class PathNoAccessException : public std::exception
{
    public:
        PathNoAccessException(const std::string &path)
        {
            msg_ = "Cannot access path: " + path;
        }

        const char *what() const noexcept override
        {
            return msg_.c_str();
        }

    private:
        std::string msg_;
};

#endif
