#pragma once

#include <functional>

#include "core.h"
#include "storedvalue.h"

#include "token.h"

namespace Dern
{
    class TypeParser
    {
    public:
        static Ref<StoredValue> ComputeValue(std::function<Ref<Token>()> nextFn);
    };
}