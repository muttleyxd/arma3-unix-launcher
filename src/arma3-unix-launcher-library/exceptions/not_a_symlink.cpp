#include "not_a_symlink.hpp"

NotASymlinkException::NotASymlinkException(std::string const &path): message("Not a symlink: " + path)
{
}

char const *NotASymlinkException::what() const noexcept
{
    return message.c_str();
}
