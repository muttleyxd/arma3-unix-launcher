#include "exceptions/directory_no_access.hpp"

DirectoryNoAccessException::DirectoryNoAccessException(std::string const &path)
{
    msg_ = "Cannot access directory" + path;
}

const char *DirectoryNoAccessException::what() const noexcept
{
    return msg_.c_str();
}
