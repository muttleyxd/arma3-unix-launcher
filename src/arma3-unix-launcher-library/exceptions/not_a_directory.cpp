#include "exceptions/not_a_directory.hpp"

NotADirectoryException::NotADirectoryException(std::string const &path)
{
    msg_ = "Not a directory: " + path;
}

const char *NotADirectoryException::what() const noexcept
{
    return msg_.c_str();
}
