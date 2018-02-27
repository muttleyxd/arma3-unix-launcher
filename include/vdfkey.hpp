#ifndef __VDFKEY_HPP
#define __VDFKEY_HPP

#include <string>

class VDFKey
{
  public:
    VDFKey(std::string path = "", std::string value = "") : Path(path), Value(value) {}

    std::string Path, Value;
};

#endif
