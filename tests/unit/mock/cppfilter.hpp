#pragma once

#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include <trompeloeil.hpp>

#include <filesystem>
#include <string>
#include <vector>

class CppFilter;

class CppFilterMock
{
public:
    CppFilterMock() { instance = this; }
    ~CppFilterMock() { instance = nullptr; }
    static inline CppFilterMock* instance;

    MAKE_MOCK2(RemoveClass, std::string(std::string const &, CppFilter&));
};

/*#define MAKE_STUBOMOCK(ret_type, class_name, method_name, args, mock_type) \
    ret_type class_name::method_name(args) \
    { \
       return mocktype::instance->method_name(args, *this); \
    }

MAKE_STUBOMOCK(std::string, CppFilter, RemoveClass, (std::string const&), CppFilterMock);*/

std::string CppFilter::RemoveClass(std::string const& class_name)
{
    return CppFilterMock::instance->RemoveClass(class_name, *this);
}

