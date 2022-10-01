#include "cmdruns.h"

namespace Dern
{
    bool CmdRun::Route(const std::string& name, CmdLine::Iterator& it, bool& active)
    {
        if (name == "--help")
        {
            Help();
            active = false;
            return false;
        }
        else if (name == "--version")
        {
            Version();
            active = false;
            return false;
        }
        else if (name == "--build")
        {
            Build();
            active = false;
            return false;
        }

        return true;
    }
}
