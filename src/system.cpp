#include "pch.h"
#include "coreinc.h"
#include "debug.h"

namespace Dern
{
    LSystem::LSystem(WeakRef<LProgram> ref)
        : m_Ref(ref)
    {
    }

    LSystem::~LSystem()
    {
    }

    bool LSystem::HasFuncDef(const std::string& name) const
    {
        return m_FuncMap.find(name) != m_FuncMap.end();
    }

    Ref<FuncDef> LSystem::GetFuncDef(const std::string& name) const
    {
        if (m_FuncMap.find(name) == m_FuncMap.end()) return nullptr;

        return m_FuncMap.at(name);
    }

    void LSystem::AddFuncDef(const std::string& name, const Ref<FuncDef>& funcDef)
    {
        m_FuncMap[name] = funcDef;
    }

    Ref<StackFrame> LSystem::GetStackFrame(int index) const
    {
        if (index < 0) return m_FrameStack.at(m_FrameStack.size() + index);
        
        return m_FrameStack.at(index);
    }

    void LSystem::AddStackFrame(const Ref<StackFrame>& frame)
    {
        m_FrameStack.push_back(frame);
    }

    Ref<StackFrame> LSystem::PopStackFrame(int index)
    {
        auto ref = m_FrameStack.at(m_FrameStack.size() - 1 - index);
        m_FrameStack.erase(m_FrameStack.begin() + (m_FrameStack.size() - 1));

        return ref;
    }

    void LSystem::SetRegistry(const Ref<Registry>& reg)
    {
        m_Registry = reg;
    }

    Ref<Registry> LSystem::GetRegistry() const
    {
        return m_Registry;
    }

    Ref<StoredValue> LSystem::Call(const std::string& name, std::vector<Ref<StoredValue>>& params)
    {
        CALL_LOG("Running Call For '" << name << "'");

        if (!m_Ref)
        {
            std::cerr << "Invalid Program Reference!";
            throw "Reference Error";
        }

        CALL_LOG("Getting Func Name(" << name << ")");
        auto func = GetFuncDef(name);
        if (func == nullptr)
        {
            throw "Invalid func name!";
        }

        CALL_LOG("Checking Size");
        if (params.size() != (size_t)func->GetParamCount())
        {
            std::cerr << "Invalidd func call argument count. Expected '" << func->GetParamCount() << "' but got '" << params.size() << "'!\n";
            return RNULL();
        }

        CALL_LOG("Creating Local Registry");
        Ref<Registry> funcLocal = Registry::Create();
        for (size_t index = 0; index < params.size(); index++)
        {
            auto sv = params.at(index);
            if (sv->IsOfType<int>())
            {
                if (func->At(index).GetType() != EType::Int)
                {
                    std::cerr << "Unexpected type for '" << name << "' argument #" << index << ". Expected " << FromTypeToString(func->At(index).GetType()) << ", got number!\n";
                    return RNULL();
                }

                auto name = func->At(index).GetName();
                auto data = sv->GetData<int>();

                CALL_LOG("Setting " << name << "=>Int(" << data << ")");
                funcLocal->SetEntry<int>(name, data);
            }
            else if (sv->IsOfType<std::string>())
            {
                if (func->At(index).GetType() != EType::Text)
                {
                    std::cerr << "Unexpected type for '" << name << "' argument #" << index << ". Expectedd " << FromTypeToString(func->At(index).GetType()) << ", got text!\n";
                    return RNULL();
                }

                auto name = func->At(index).GetName();
                auto data = sv->GetData<std::string>();

                CALL_LOG("Setting " << name << "=>Text(" << data << ")");
                funcLocal->SetEntry<std::string>(name, data);
            }
            else
            {
                std::cerr << "Unexpected data type for '" << name << "' argument #" << index << "!\n";
                return RNULL();
            }
        }

        auto call = Ref<FuncCall>::Create(m_Ref->m_InstructionIndex, func, m_Registry, funcLocal);
        auto frame = Ref<CallStackFrame>::Create(funcLocal, call);
        m_FrameStack.push_back(frame);
        m_Ref->m_InstructionIndex = func->GetStart();
        m_Registry = funcLocal;

        CALL_LOG(call->ToString() << " " << m_Ref->m_InstructionIndex);
        
        return m_Ref->RunImpl();
    }

    TypeParser LSystem::CreateParser(int memory)
    {
        return TypeParser(this, memory);
    }
}