#pragma once

#include "funccall.h"
#include "registry.h"

namespace Dern
{
    enum class StackFrameType
    {
        Global, Call, Scope
    };

    class StackFrame
    {
    public:
        StackFrame(const Ref<Registry>& reg);
        virtual ~StackFrame() = default;

        const Ref<Registry>& GetRegistry() const { return m_Reg; }

        virtual StackFrameType GetType() const = 0;
    private:
        Ref<Registry> m_Reg;
    };

    class GlobalStackFrame : public StackFrame
    {
    public:
        GlobalStackFrame(const Ref<Registry>& reg);
        virtual ~GlobalStackFrame() = default;

        virtual StackFrameType GetType() const override { return StackFrameType::Global; }
    };

    class CallStackFrame : public StackFrame
    {
    public:
        CallStackFrame(const Ref<Registry>& reg, const Ref<FuncCall>& funcCall);
        virtual ~CallStackFrame() = default;

        const Ref<FuncCall>& GetFuncCall() const { return m_FuncCall; }

        virtual StackFrameType GetType() const override { return StackFrameType::Call; }
    private:
        Ref<FuncCall> m_FuncCall;
    };
    
    class ScopeStackFrame : public StackFrame
    {
    public:
        ScopeStackFrame(const Ref<Registry>& reg);
        virtual ~ScopeStackFrame() = default;

        virtual StackFrameType GetType() const override { return StackFrameType::Scope; }
    };
}