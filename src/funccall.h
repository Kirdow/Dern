#pragma once

#include "funcdef.h"
#include "registry.h"

namespace Dern
{
    class FuncCall
    {
    public:
        FuncCall(int returnIndex, Ref<FuncDef> def, Ref<Registry> parentReg, Ref<Registry> reg)
            : m_ReturnIndex(returnIndex), m_Func(def), m_ParentReg(parentReg), m_Reg(reg) {}
        ~FuncCall() {}

        int GetReturnIndex() const { return m_ReturnIndex; }
        const Ref<FuncDef>& GetFuncDef() const { return m_Func; }
        const Ref<Registry>& GetParentRegistry() const { return m_ParentReg; }
        const Ref<Registry>& GetRegistry() const { return m_Reg; }

        std::string ToString() const;
    private:
        int m_ReturnIndex;
        Ref<FuncDef> m_Func;
        Ref<Registry> m_ParentReg;
        Ref<Registry> m_Reg;
    };
}