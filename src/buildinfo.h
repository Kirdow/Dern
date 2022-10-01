#pragma once

namespace Dern
{
    class BuildInfo
    {
    public:
        static const char* const Name;
        static const char* const Version;
        static const char* const Build;

        static const char* const Repo;

        static const char* GetAppName();
    };
}