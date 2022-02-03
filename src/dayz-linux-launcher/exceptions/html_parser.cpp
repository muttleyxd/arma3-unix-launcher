#include "html_parser.hpp"

HtmlParserException::HtmlParserException(std::string const &error) : message("HTML Parser error: " +
            error)
{
}

char const *HtmlParserException::what() const noexcept
{
    return message.c_str();
}
