#pragma once

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
        size_t GetColonNewlineOrChar(size_t pos);
};
