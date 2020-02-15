#include "file_not_found.hpp"

FileNotFoundException::FileNotFoundException(std::string const &path) : message("File not found: " + path)
{
}

const char *FileNotFoundException::what() const noexcept
{
    return message.c_str();
}
