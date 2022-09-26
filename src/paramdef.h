#pragma once

#include <string>

namespace Dern
{
    enum class EType
    {
        Int, Text, Void
    };

    std::string FromTypeToString(EType type);
    EType FromStringToType(const std::string& str);

    class ParamDef
    {
    public:
        ParamDef() : ParamDef(EType::Void, "") {}
        ParamDef(EType type, const std::string& name) : m_Type(type), m_Name(name) {}
        ~ParamDef() {}

        EType GetType() const { return m_Type; }
        const std::string& GetName() const { return m_Name; }

        std::string ToString() const;
    private:
        EType m_Type;
        std::string m_Name;
    };
}