#ifndef __EXCEPTIONS_DIRECTORY_NO_ACCESS_HPP
#define __EXCEPTIONS_DIRECTORY_NO_ACCESS_HPP

#include <exception>

class DirectoryNoAccessException : public std::exception
{
    public:
        DirectoryNoAccessException(const std::string &path)
        {
            msg_ = "Cannot access directory" + path;
        }

        const char *what() const noexcept override
        {
            return msg_.c_str();
        }

    private:
        std::string msg_;
};

#endif
