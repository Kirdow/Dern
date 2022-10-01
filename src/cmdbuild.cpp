#include "cmdruns.h"
#include "buildinfo.h"

namespace Dern
{
    void CmdRun::Build()
    {
        std::cout << Dern::BuildInfo::Name << " Version " << Dern::BuildInfo::Version << " (Build " << Dern::BuildInfo::Build << ")\n";
    }
}