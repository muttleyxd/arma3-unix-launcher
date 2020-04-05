#include "not_a_directory.hpp"

NotADirectoryException::NotADirectoryException(std::string const &path): message("Not a directory: " + path)
{
}

char const *NotADirectoryException::what() const noexcept
{
    return message.c_str();
}
