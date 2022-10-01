#pragma once

#include "core.h"

//#define ENABLE_DEBUG_LOCATION
//#define ENABLE_DEBUG_READ
//#define ENABLE_DEBUG_RESULT
//#define ENABLE_DEBUG_LOG
//#define ENABLE_CALL_LOG

namespace Dern
{
    class Debug
    {
    public:
        Debug(void* ref);
        ~Debug();

        static int GetInstructionIndex();
    private:
        template<typename T>
        const T* Ref() const
        {
            if (!RefUtil::CheckReference(m_Ref))
                return nullptr;
            return (const T*)m_Ref;
        }
    private:
        void* m_Ref;
        static Debug* s_Instance;
    };
}

#ifdef ENABLE_DEBUG_LOCATION
    #define DEBUG_BASE "(" << __FILE__ << ":" << __func__ << ":" << __LINE__ << ") "
#else
    #define DEBUG_BASE ""
#endif


#ifdef ENABLE_DEBUG_READ
    #define DEBUG_READ(token) std::cout << DEBUG_BASE << "Read[" << Debug::GetInstructionIndex() << "]: " << *(token) << "\n"
#else
    #define DEBUG_READ(token)
#endif

#ifdef ENABLE_DEBUG_RESULT
    #define DEBUG_RESULT(data, type) std::cout << DEBUG_BASE << "Result[" << Debug::GetInstructionIndex() << "]: " << type << "(" << data << ")\n";
#else
    #define DEBUG_RESULT(data, type)
#endif

#ifdef ENABLE_DEBUG_LOG
    #define DEBUG_LOG(stream) std::cout << DEBUG_BASE << "Log[" << Debug::GetInstructionIndex() << "]: " << stream << "\n";
#else
    #define DEBUG_LOG(stream)
#endif

#ifdef ENABLE_CALL_LOG
    #define CALL_LOG(stream) std::cout << DEBUG_BASE << "Call Log: " << stream << "\n";
#else
    #define CALL_LOG(stream)
#endif