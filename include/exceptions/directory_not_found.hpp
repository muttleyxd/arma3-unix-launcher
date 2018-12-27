#ifndef __EXCEPTIONS_DIRECTORY_NOT_FOUND_HPP
#define __EXCEPTIONS_DIRECTORY_NOT_FOUND_HPP

#include <exception>

class DirectoryNotFoundException : public std::exception
{
    public:
        DirectoryNotFoundException(std::string path)
        {
            msg_ = "Directory not found: " + path;
        }

        const char *what() const noexcept override
        {
            return msg_.c_str();
        }

    private:
        std::string msg_;
};

#endif
