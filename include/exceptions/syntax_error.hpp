#ifndef __EXCEPTIONS_SYNTAX_ERROR_HPP
#define __EXCEPTIONS_SYNTAX_ERROR_HPP

#include <exception>

class SyntaxErrorException : public std::exception
{
    public:
        SyntaxErrorException(const std::string &error)
        {
            msg_ = "Syntax error: " + error;
        }

        const char *what() const noexcept override
        {
            return msg_.c_str();
        }

    private:
        std::string msg_;
};

#endif
