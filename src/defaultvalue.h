#pragma once

#include <string>

namespace Dern::Defaults
{
    template<typename T>
    inline T GetDefault() { }
    template<>
    inline int GetDefault<int>() { return 0; }
    template<>
    inline std::string GetDefault<std::string>() { return std::string(); }
}