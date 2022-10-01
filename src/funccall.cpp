#include "funccall.h"

namespace Dern
{
    std::string FuncCall::ToString() const
    {
        std::stringstream sstr;

        sstr << "Call#" << this << "[" << m_ReturnIndex << "|" << m_ParentReg.Raw() << "|" << m_Reg.Raw() << "|" << m_Func->ToString() << "]";

        return sstr.str();
    }
}