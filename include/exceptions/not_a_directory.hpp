#ifndef __EXCEPTIONS_NOT_A_DIRECTORY_HPP
#define __EXCEPTIONS_NOT_A_DIRECTORY_HPP

#include <exception>

class NotADirectoryException : public std::exception
{
    public:
        NotADirectoryException(std::string path)
        {
            msg_ = "Not a directory: " + path;
        }

        const char *what() const noexcept override
        {
            return msg_.c_str();
        }

    private:
        std::string msg_;
};

#endif
