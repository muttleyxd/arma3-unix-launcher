#ifndef __EXCEPTIONS_NOT_A_DIRECTORY_HPP
#define __EXCEPTIONS_NOT_A_DIRECTORY_HPP

#include <exception>

class NotADirectoryException : public std::exception
{
    public:
        NotADirectoryException(std::string path) : path_(path) {}

        const char *what() const throw()
        {
            return ("Not a directory: " + path_).c_str();
        }

    private:
        std::string path_;
};

#endif
