#include "exceptions/path_no_access.hpp"

PathNoAccessException::PathNoAccessException(std::string const &path)
{
    msg_ = "Cannot access path: " + path;
}

const char *PathNoAccessException::PathNoAccessException::what() const noexcept
{
    return msg_.c_str();
}
