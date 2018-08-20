#ifndef __EXCEPTIONS_NOT_A_SYMLINK_HPP
#define __EXCEPTIONS_NOT_A_SYMLINK_HPP

#include <exception>

class NotASymlinkException : public std::exception
{
    public:
        NotASymlinkException(std::string path)
        {
            msg_ = "Not a symlink: " + path;
        }

        const char *what() const noexcept override
        {
            return msg_.c_str();
        }

    private:
        std::string msg_;
};

#endif
