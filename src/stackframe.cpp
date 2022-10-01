#include "stackframe.h"
#include "registry.h"

namespace Dern
{
    // Base
    StackFrame::StackFrame(const Ref<Registry>& reg)
        : m_Reg(reg) {}

    GlobalStackFrame::GlobalStackFrame(const Ref<Registry>& reg)
        : StackFrame(reg) {}
    
    CallStackFrame::CallStackFrame(const Ref<Registry>& reg, const Ref<FuncCall>& funcCall)
        : StackFrame(reg), m_FuncCall(funcCall) {}
    
    ScopeStackFrame::ScopeStackFrame(const Ref<Registry>& reg)
        : StackFrame(reg) {}
}