#pragma once

#include "storedvalue.h"

namespace Dern::ValueHelper
{
    Ref<StoredValue> Null();
    Ref<StoredValue> Int(int value);
    Ref<StoredValue> Text(const std::string& value);
}

#define RNULL() Dern::ValueHelper::Null()
#define RINT(value) Dern::ValueHelper::Int(value)
#define RTEXT(value) Dern::ValueHelper::Text(value)
