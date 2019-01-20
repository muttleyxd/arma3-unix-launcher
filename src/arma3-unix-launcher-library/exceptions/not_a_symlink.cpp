#include "exceptions/not_a_symlink.hpp"

NotASymlinkException::NotASymlinkException(std::string const &path)
{
    msg_ = "Not a symlink: " + path;
}

const char *NotASymlinkException::what() const noexcept
{
    return msg_.c_str();
}
