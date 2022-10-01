#pragma once

#include "cmd.h"

namespace Dern
{
    class CmdRun
    {
    public:
        static void Version();
        static void Build();
        static void Help();

        static bool Route(const std::string& name, CmdLine::Iterator& it, bool& active);
    };
}