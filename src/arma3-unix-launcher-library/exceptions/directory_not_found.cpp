#include "exceptions/directory_not_found.hpp"

DirectoryNotFoundException::DirectoryNotFoundException(std::string const &path)
{
    msg_ = "Directory not found: " + path;
}

const char *DirectoryNotFoundException::what() const noexcept
{
    return msg_.c_str();
}
