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
        ModMock()
        {
            instance = this;
        }
        ~ModMock()
        {
            instance = nullptr;
        }
        static inline ModMock *instance;

        MAKE_MOCK2(Constructor, void(std::filesystem::path const &, Mod &));
        MAKE_CONST_MOCK1(GetName, std::string(Mod const &));
        MAKE_MOCK1(LoadAllCPP, void(Mod &));
        MAKE_MOCK3(LoadFromText, void(std::string const &, bool, Mod &));
        MAKE_CONST_MOCK2(IsWorkshopMod, bool(std::filesystem::path const &, Mod const &));
};

Mod::Mod(std::filesystem::path const &path) : path_(path)
{
    ModMock::instance->Constructor(path, *this);
}

std::string Mod::GetName() const
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

bool Mod::IsWorkshopMod(std::filesystem::path const &workshop_path) const
{
    return ModMock::instance->IsWorkshopMod(workshop_path, *this);
}
