#include "exceptions/file_not_found.hpp"

FileNotFoundException::FileNotFoundException(std::string const &path)
{
    msg_ = "File not found: " + path;
}

const char *FileNotFoundException::what() const noexcept
{
    return msg_.c_str();
}
