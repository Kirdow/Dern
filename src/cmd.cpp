#include "cmd.h"

namespace Dern
{
    CmdLine::CmdLine(int argc, char** argv)
        : m_Argc(argc)
    {
        for (int i = 0; i < argc; i++)
        {
            std::string arg = argv[i];
            m_Args.push_back(arg);
        }
    }

    CmdLine::~CmdLine() {}

    bool CmdLine::GetNext(size_t index, std::string& result) const
    {
        size_t nextIndex = index + 1;
        if (nextIndex >= m_Argc) return false;

        result = m_Args.at(nextIndex);
        return true;
    }
}