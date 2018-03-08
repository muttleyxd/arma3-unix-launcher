#ifndef __MOD_HPP
#define __MOD_HPP

#include <map>
#include <string>

class Mod
{
  public:
    Mod(std::string path);

    std::map<std::string, std::string> KeyValue;

    int LoadFromFile(std::string path, bool append = false);
    int LoadFromText(std::string text, bool append = false);
};

#endif
