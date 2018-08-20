#ifndef __STD_UTILS_HPP
#define __STD_UTILS_HPP

#include <algorithm>
#include <map>
#include <string>
#include <vector>

namespace StdUtils
{
    template<typename T, typename vector = std::vector<T>>
    bool Contains(const vector &vec, const T &t)
    {
        return std::any_of(vec.begin(), vec.end(), std::bind2nd(std::equal_to<T>(), t));
    }

    bool Contains(const std::vector<std::string> &vec, const char *t)
    {
        return std::any_of(vec.begin(), vec.end(), std::bind2nd(std::equal_to<std::string>(), std::string(t)));
    }

    template<typename T, typename X>
    bool ContainsKey(const std::map<T, X> &map, const T &key)
    {
        return map.find(key) != map.end();
    }

    template<typename T>
    bool ContainsKey(const std::map<std::string, T> &map, const char *key)
    {
        return map.find(std::string(key)) != map.end();
    }
}

#endif
