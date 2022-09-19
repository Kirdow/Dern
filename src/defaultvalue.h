#pragma once

#include <string>

namespace Dern::Defaults
{
    template<typename T>
    T GetDefault() { }
    template<>
    int GetDefault<int>() { return 0; }
    template<>
    std::string GetDefault<std::string>() { return std::string(); }
}