#include "pch.h"
#include "paramdef.h"

namespace Dern
{
    std::string FromTypeToString(EType type)
    {
        switch (type)
        {
        case EType::Int: return "number";
        case EType::Text: return "text";
        case EType::Void: return "void";
        }

        throw "Unknown type";
    }

    EType FromStringToType(const std::string& str)
    {
        if (str == "number") return EType::Int;
        else if (str == "text") return EType::Text;
        else if (str == "void") return EType::Void;

        throw "Unknown type string";
    }

    std::string ParamDef::ToString() const
    {
        std::stringstream sstr;
        sstr << "$" << FromTypeToString(m_Type) << ":" << m_Name;
        return sstr.str();
    }
}