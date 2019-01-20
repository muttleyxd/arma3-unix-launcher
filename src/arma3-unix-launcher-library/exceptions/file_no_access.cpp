#include "exceptions/file_no_access.hpp"

FileNoAccessException::FileNoAccessException(const std::string &path)
{
    msg_ = "Cannot access file: " + path;
}

const char *FileNoAccessException::what() const noexcept
{
    return msg_.c_str();
}
