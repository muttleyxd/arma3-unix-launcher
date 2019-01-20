#ifndef EXCEPTIONS_SYNTAX_ERROR_HPP_
#define EXCEPTIONS_SYNTAX_ERROR_HPP_

#include <exception>
#include <string>

class SyntaxErrorException : public std::exception
{
    public:
        SyntaxErrorException(std::string const &error);

        const char *what() const noexcept override;

    private:
        std::string msg_;
};

#endif
