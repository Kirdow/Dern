#pragma once

#include <string>
#include <vector>

#include "paramdef.h"

namespace Dern
{
    class FuncDef
    {
    public:
        FuncDef(const std::string& name, EType returnType, const std::vector<ParamDef>& v, int bodyStart, int bodyEnd);
        FuncDef(const FuncDef&) = delete; // disable copying of funcdef
        ~FuncDef();

        const std::string& GetName() const { return m_Name; }
        int GetParamCount() const { return m_ParamCount; }
        const ParamDef& At(int index) const { return m_Params[index]; }
        EType GetReturnType() const { return m_ReturnType; }

        int GetStart() const { return m_BodyStart; }
        int GetEnd() const { return m_BodyEnd; }

        std::string ToString() const;
    private:
        std::string m_Name;
        int m_ParamCount;
        ParamDef* m_Params;
        EType m_ReturnType;

        int m_BodyStart;
        int m_BodyEnd;
    };
}