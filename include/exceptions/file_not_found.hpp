#ifndef EXCEPTIONS_FILE_NOT_FOUND_HPP_
#define EXCEPTIONS_FILE_NOT_FOUND_HPP_

#include <exception>
#include <string>

class FileNotFoundException : public std::exception
{
    public:
        FileNotFoundException(std::string const &path);

        const char *what() const noexcept override;

    private:
        std::string msg_;
};

#endif
