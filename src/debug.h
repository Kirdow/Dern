#pragma once

#include "core.h"

//#define ENABLE_DEBUG_LOCATION
//#define ENABLE_DEBUG_READ
//#define ENABLE_DEBUG_RESULT
//#define ENABLE_DEBUG_LOG
//#define ENABLE_CALL_LOG

#define DEBUG_COLOR(n) "\u001b[" << 3 << n << "m"
#define DEBUG_CRESET "\u001b[0m"
#define DEBUG_RED DEBUG_COLOR(1)
#define DEBUG_GREEN DEBUG_COLOR(2)
#define DEBUG_BLUE DEBUG_COLOR(4)
#define DEBUG_YELLOW DEBUG_COLOR(3)
#define DEBUG_MAGENTA DEBUG_COLOR(5)

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
    #define DEBUG_BASE DEBUG_YELLOW << "(" << __FILE__ << ":" << __func__ << ":" << __LINE__ << ") " << DEBUG_CRESET
#else
    #define DEBUG_BASE ""
#endif


#ifdef ENABLE_DEBUG_READ
    #define DEBUG_READ(token) std::cout << DEBUG_BASE << DEBUG_RED << "Read[" << Debug::GetInstructionIndex() << "]: " << *(token) << DEBUG_CRESET << "\n"
#else
    #define DEBUG_READ(token)
#endif

#ifdef ENABLE_DEBUG_RESULT
    #define DEBUG_RESULT(data, type) std::cout << DEBUG_BASE << DEBUG_GREEN << "Result[" << Debug::GetInstructionIndex() << "]: " << type << "(" << data << ")" << DEBUG_CRESET << "\n";
#else
    #define DEBUG_RESULT(data, type)
#endif

#ifdef ENABLE_DEBUG_LOG
    #define DEBUG_LOG(stream) std::cout << DEBUG_BASE << DEBUG_MAGENTA << "Log[" << Debug::GetInstructionIndex() << "]: " << stream << DEBUG_CRESET << "\n";
#else
    #define DEBUG_LOG(stream)
#endif

#ifdef ENABLE_CALL_LOG
    #define CALL_LOG(stream) std::cout << DEBUG_BASE << DEBUG_BLUE << "Call Log: " << stream << DEBUG_CRESET << "\n";
#else
    #define CALL_LOG(stream)
#endif