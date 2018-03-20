#include "mod.hpp"

Mod::Mod(const std::string &path)
{
}

int Mod::LoadFromFile(const std::string &path, bool append)
{
    errno = ENOSYS;
    return -1;
}

int LoadFromText(const std::string &text, bool append)
{
    errno = ENOSYS;
    return -1;
}
