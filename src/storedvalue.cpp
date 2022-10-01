#include "storedvalue.h"

namespace Dern
{
    std::string StoredValue::ToString() const
    {
        std::stringstream sstr;
        sstr << "SValue#" << this << "[";
        if (!m_Name.empty()) sstr << m_Name << "=";
        if (IsOfType<int>())
            sstr << "Int(" << GetData<int>() << ")";
        else if (IsOfType<std::string>())
            sstr << "Text(" << GetData<std::string>() << ")";
        else
            sstr << "None";
        sstr << "]";
        return sstr.str();
    }
}