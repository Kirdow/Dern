#pragma once

#include <memory>

#define DERN_EXPAND_MACRO(x) x
#define DERN_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << (x))

namespace Dern
{
    template<typename T>
    using Scope = std::unique_ptr<T>;
    template<typename T, typename ... Args>
    constexpr Scope<T> CreateScope(Args&& ... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }
}

#include "ref.h"
#include "debug.h"
