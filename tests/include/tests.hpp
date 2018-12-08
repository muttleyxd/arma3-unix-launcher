#ifndef TESTS_HPP_
#define TESTS_HPP_

#ifndef DOCTEST_CONFIG_DISABLE

#include <map>
#include <string>
#include <vector>

namespace Tests::Utils
{
    std::string RemoveLastElement(std::string s, bool removeSlash, int count);
    std::string GetWorkDir();
    std::vector<std::string> Ls(std::string path);
    extern std::map<std::string, std::string> remove_stamina_map;
    extern std::map<std::string, std::string> big_mod_map;
}

#endif

#endif
