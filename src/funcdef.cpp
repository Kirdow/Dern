#include "funcdef.h"

namespace Dern
{
    FuncDef::FuncDef(const std::string& name, EType returnType,  const std::vector<ParamDef>& v, int bodyStart, int bodyEnd)
        : m_Name(name), m_ParamCount(0), m_Params(nullptr), m_ReturnType(returnType), m_BodyStart(bodyStart), m_BodyEnd(bodyEnd)
    {
        if (v.size() > 0)
        {
            m_ParamCount = v.size();
            m_Params = new ParamDef[m_ParamCount];

            for (int index = 0; index < m_ParamCount; index++)
            {
                m_Params[index] = ParamDef(v.at(index));
            }
        }
    }

    FuncDef::~FuncDef()
    {
        if (m_ParamCount > 0 && m_Params)
        {
            delete[] m_Params;
            m_Params = nullptr;
            m_ParamCount = 0;
        }
    }

    std::string FuncDef::ToString() const
    {
        std::stringstream sstr;

        sstr << "Func#" << this << "[" << m_Name << "->" << FromTypeToString(m_ReturnType) << "|" << m_ParamCount;
        for (int i = 0; i < m_ParamCount; i++)
        {
            sstr << "," << m_Params[i].ToString();
        }
        sstr << "|" << m_BodyStart << ":" << m_BodyEnd << "]";
        
        return sstr.str();
    }

    
}