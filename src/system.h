#pragma once

namespace Dern
{
    class LProgram;
    class Registry;
    class FuncDef;
    class FuncCall;
    class StackFrame;
    class StoredValue;
    class TypeParser;
    class LSystem
    {
    public:
        LSystem(WeakRef<LProgram> ref);
        ~LSystem();

        // Func Def
        bool HasFuncDef(const std::string& name) const;
        Ref<FuncDef> GetFuncDef(const std::string& name) const;
        void AddFuncDef(const std::string& name, const Ref<FuncDef>& funcDef);

        // Frame Stack
        int FrameStackSize() const { return m_FrameStack.size(); }
        Ref<StackFrame> GetStackFrame(int index) const;
        void AddStackFrame(const Ref<StackFrame>& frame);
        Ref<StackFrame> PopStackFrame(int index = 0);

        // Registry
        void SetRegistry(const Ref<Registry>& reg);
        Ref<Registry> GetRegistry() const;

        // Func calls
        Ref<StoredValue> Call(const std::string& name, std::vector<Ref<StoredValue>>& params);

        // Parser
        TypeParser CreateParser(int memory);
    private:
        WeakRef<LProgram> m_Ref;
        Ref<Registry> m_Registry;
        std::unordered_map<std::string, Ref<FuncDef>> m_FuncMap;
        std::vector<Ref<StackFrame>> m_FrameStack;
    };
}