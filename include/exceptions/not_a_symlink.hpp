#ifndef __EXCEPTIONS_NOT_A_SYMLINK_HPP
#define __EXCEPTIONS_NOT_A_SYMLINK_HPP

#include <exception>

class NotASymlinkException : public std::exception
{
    public:
        NotASymlinkException(std::string path) : path_(path) {}

        const char *what() const throw()
        {
            return ("Not a symlink: " + path_).c_str();
        }

    private:
        std::string path_;
};

#endif
