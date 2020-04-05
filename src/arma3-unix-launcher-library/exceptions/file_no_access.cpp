#include "file_no_access.hpp"

FileNoAccessException::FileNoAccessException(std::string const &path): message("Cannot access file: " + path)
{
}

char const *FileNoAccessException::what() const noexcept
{
    return message.c_str();
}
