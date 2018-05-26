#ifndef __EXCEPTIONS_DIRECTORY_NO_ACCESS_HPP
#define __EXCEPTIONS_DIRECTORY_NO_ACCESS_HPP

#include <exception>

class DirectoryNoAccessException : public std::exception
{
    public:
        DirectoryNoAccessException(const std::string& path) : path_(path) {}

        const char *what() const throw()
        {
            return ("Cannot access directory: " + path_).c_str();
        }

    private:
        std::string path_;
};

#endif
