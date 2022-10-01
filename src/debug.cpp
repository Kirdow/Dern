#include "debug.h"

#include "program.h"

namespace Dern
{
    Debug* Debug::s_Instance = nullptr;

    Debug::Debug(void* ref)
        : m_Ref(ref)
    {
        if (s_Instance == nullptr) s_Instance = this;
    }

    Debug::~Debug() {}

    int Debug::GetInstructionIndex()
    {
        const int _default = 0;
        if (!s_Instance) return _default;

        auto ref = s_Instance->Ref<LProgram>();
        if (!ref) return _default;

        return ref->GetIndex();
    }

    
}