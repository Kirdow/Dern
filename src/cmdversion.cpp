#include "cmdruns.h"
#include "buildinfo.h"

namespace Dern
{
    void CmdRun::Version()
    {
        std::cout << Dern::BuildInfo::Name << " Version " << Dern::BuildInfo::Version << " (Build " << Dern::BuildInfo::Build << ")\n";
        std::cout << "Git Repository: " << Dern::BuildInfo::Repo << "\n";
    }
}