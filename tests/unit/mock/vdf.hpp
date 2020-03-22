#pragma once

#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include <trompeloeil.hpp>

#include <filesystem>
#include <string>
#include <vector>

class VDF;

class VdfMock
{
    public:
        VdfMock()
        {
            instance = this;
        }
        ~VdfMock()
        {
            instance = nullptr;
        }
        static inline VdfMock *instance;

        MAKE_MOCK3(LoadFromText, void(std::string_view const, bool, VDF &));
        MAKE_MOCK2(GetValuesWithFilter, std::vector<std::string>(std::string_view const, VDF &));
};

std::vector<std::string> VDF::GetValuesWithFilter(std::string_view const filter)
{
    return VdfMock::instance->GetValuesWithFilter(filter, *this);
}

void VDF::LoadFromText(std::string_view const text, bool append)
{
    VdfMock::instance->LoadFromText(text, append, *this);
}
