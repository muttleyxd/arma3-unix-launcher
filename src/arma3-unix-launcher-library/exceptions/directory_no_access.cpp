#include "directory_no_access.hpp"

DirectoryNoAccessException::DirectoryNoAccessException(std::string const &path) : message("Cannot access directory" +
            path)
{
}

char const *DirectoryNoAccessException::what() const noexcept
{
    return message.c_str();
}
