#include "vdf.hpp"

VDF::VDF(std::string path)
{
}

std::vector<std::string> VDF::GetValuesWithFilter(std::string filter)
{
    std::vector<std::string> result;
    return std::move(result);
}

int VDF::LoadFromFile(std::string path, bool append)
{
    errno = ENOSYS;
    return -1;
}

int LoadFromText(std::string text, bool append)
{
    errno = ENOSYS;
    return -1;
}
