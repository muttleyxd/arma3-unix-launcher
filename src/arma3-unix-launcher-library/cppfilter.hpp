#ifndef CPP_FILTER_HPP_
#define CPP_FILTER_HPP_

#include <string>
#include <vector>

class CppFilter
{
public:
    std::string RemoveClass(std::string const &class_name);

    std::string class_text_;

private:
    std::vector<size_t> FindAllClassOccurences(std::string const &class_name);
    std::pair<size_t, size_t> GetClassBoundaries(std::string const &class_name, size_t start);
};

#endif
