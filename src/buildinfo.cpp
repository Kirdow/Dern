#include "buildinfo.h"

extern char* s_AppName;

namespace Dern
{
    const char* const BuildInfo::Name = BUILD_NAME;
    const char* const BuildInfo::Version = BUILD_VERSION;
    const char* const BuildInfo::Build = BUILD_NUMBER;
    
    const char* const BuildInfo::Repo = "https://github.com/Kirdow/Dern";

    const char* BuildInfo::GetAppName() { return s_AppName; }
}