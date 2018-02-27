#ifndef __VDF_HPP
#define __VDF_HPP

#include "vdfkey.hpp"

#include <map>
#include <string>
#include <vector>

class VDF
{
  public:
    VDF(std::string path);

    std::map<std::string, std::string> KeyValue;
    std::vector<std::string> GetValuesWithFilter(std::string filter);
};

#endif
