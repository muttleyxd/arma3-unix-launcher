#ifndef __VDF_HPP
#define __VDF_HPP

#include "vdfkey.hpp"

#include <map>
#include <string>
#include <vector>

class VDF
{
  public:
    VDF(std::string path = "");

    int LoadFromFile(std::string path, bool append = false);
    int LoadFromText(std::string text, bool append = false);

    std::map<std::string, std::string> KeyValue;
    std::vector<std::string> GetValuesWithFilter(std::string filter);
};

#endif
