#include "valuehelper.h"

namespace Dern::ValueHelper
{
    Ref<StoredValue> Null() { return Ref<StoredValue>::Create(); }
    
    Ref<StoredValue> Int(int value)
    {
        auto result = Ref<StoredValue>::Create();
        result->SetData<int>(value);
        return result;
    }

    Ref<StoredValue> Text(const std::string& value)
    {
        auto result = Ref<StoredValue>::Create();
        result->SetData<std::string>(value);
        return result;
    }
}