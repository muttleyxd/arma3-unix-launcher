#include "syntax_error.hpp"

SyntaxErrorException::SyntaxErrorException(std::string const &error): message("Syntax error: " + error)
{
}

char const *SyntaxErrorException::what() const noexcept
{
    return message.c_str();
}
