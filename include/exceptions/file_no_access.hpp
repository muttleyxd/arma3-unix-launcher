#ifndef __EXCEPTIONS_FILE_NO_ACCESS_HPP
#define __EXCEPTIONS_FILE_NO_ACCESS_HPP

#include <exception>

class FileNoAccessException : public std::exception
{
    public:
        FileNoAccessException(const std::string& path) : path_(path) {}

        const char *what() const throw()
        {
            return ("Cannot access file: " + path_).c_str();
        }

    private:
        std::string path_;
};

#endif
