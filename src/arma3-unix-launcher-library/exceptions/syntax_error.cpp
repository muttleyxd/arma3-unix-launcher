#include "exceptions/syntax_error.hpp"

SyntaxErrorException::SyntaxErrorException(std::string const &error)
{
    msg_ = "Syntax error: " + error;
}

const char *SyntaxErrorException::what() const noexcept
{
    return msg_.c_str();
}
