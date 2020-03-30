#include "directory_not_found.hpp"

DirectoryNotFoundException::DirectoryNotFoundException(std::string const &path) : message("Directory not found: " +
            path)
{
}

char const *DirectoryNotFoundException::what() const noexcept
{
    return message.c_str();
}
