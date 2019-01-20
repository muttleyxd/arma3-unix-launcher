#ifndef EXCEPTIONS_NOT_A_SYMLINK_HPP_
#define EXCEPTIONS_NOT_A_SYMLINK_HPP_

#include <exception>
#include <string>

class NotASymlinkException : public std::exception
{
    public:
        NotASymlinkException(std::string const &path);

        const char *what() const noexcept override;

    private:
        std::string msg_;
};

#endif
