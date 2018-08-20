#ifndef __EXCEPTIONS_FILE_NOT_FOUND_HPP
#define __EXCEPTIONS_FILE_NOT_FOUND_HPP

#include <exception>

class FileNotFoundException : public std::exception
{
    public:
        FileNotFoundException(std::string path)
        {
            msg_ = "File not found: " + path;
        }

        const char *what() const noexcept override
        {
            return msg_.c_str();
        }

    private:
        std::string msg_;
};

#endif
