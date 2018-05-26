#ifndef __EXCEPTIONS_SYNTAX_ERROR_HPP
#define __EXCEPTIONS_SYNTAX_ERROR_HPP

#include <exception>

class SyntaxErrorException : public std::exception
{
    public:
        SyntaxErrorException(const std::string& error) : error_(error) {}

        const char *what() const throw()
        {
            return ("Syntax error: " + error_).c_str();
        }

    private:
        std::string error_;
};

#endif
