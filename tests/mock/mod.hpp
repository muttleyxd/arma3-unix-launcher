#pragma once

#include <doctest/doctest.h>
#include <doctest/trompeloeil.hpp>
#include <trompeloeil.hpp>

#include <filesystem>
#include <string>
#include <vector>

class Mod;

class ModMock
{
public:
    ModMock() { instance = this; }
    ~ModMock() { instance = nullptr; }
    static inline ModMock* instance;

    MAKE_MOCK2(Constructor, void(std::filesystem::path const&, Mod&));
    MAKE_MOCK1(GetName, std::string(Mod&));
    MAKE_MOCK1(LoadAllCPP, void(Mod&));
    MAKE_MOCK3(LoadFromText, void(std::string const&, bool, Mod&));
};

Mod::Mod(std::filesystem::path const& path) : path_(path)
{
    ModMock::instance->Constructor(path, *this);
}

std::string Mod::GetName()
{
    return ModMock::instance->GetName(*this);
}

void Mod::LoadAllCPP()
{
    ModMock::instance->LoadAllCPP(*this);
}

void Mod::LoadFromText(std::string const &text, bool append)
{
    ModMock::instance->LoadFromText(text, append, *this);
}
