#ifndef __EXCEPTIONS_PATH_NO_ACCESS_HPP
#define __EXCEPTIONS_PATH_NO_ACCESS_HPP

#include <exception>

class PathNoAccessException : public std::exception
{
    public:
        PathNoAccessException(const std::string& path) : path_(path) {}

        const char *what() const throw()
        {
            return ("Cannot access path: " + path_).c_str();
        }

    private:
        std::string path_;
};

#endif
