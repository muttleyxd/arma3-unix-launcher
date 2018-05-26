#ifndef __EXCEPTIONS_FILE_NOT_FOUND_HPP
#define __EXCEPTIONS_FILE_NOT_FOUND_HPP

#include <exception>

class FileNotFoundException : public std::exception
{
    public:
        FileNotFoundException(std::string path) : path_(path) {}

        const char *what() const throw()
        {
            return ("File not found: " + path_).c_str();
        }

    private:
        std::string path_;
};

#endif
