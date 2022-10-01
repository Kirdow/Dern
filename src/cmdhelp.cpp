#include "cmdruns.h"
#include "buildinfo.h"
#include "strutil.h"

namespace Dern
{
    struct AlignStream
    {
        int AlignColumn;

        void Reset() { AlignColumn = 0; }
        void Align(int max)
        {
            std::stringstream sstr;
            while (AlignColumn < max)
            {
                sstr << " ";
                ++AlignColumn;
            }
            std::cout << sstr.str();
        }

        template<typename T>
        void Print(const T& data)
        {
            std::stringstream sstr;
            sstr << data;
            auto str = sstr.str();

            if (StrUtil::EndsWith(str, "\n"))
            {
                std::cout << str;
                Reset();
                return;
            }

            AlignColumn += str.length();
            std::cout << str;
        }
    };

    template<typename T>
    AlignStream& operator<<(AlignStream& astr, const T& data)
    {
        astr.Print<T>(data);

        return astr;
    }

    void CmdRun::Help()
    {
        const char* dernapp = Dern::BuildInfo::GetAppName();

        AlignStream astr;
        astr << "Usage: " << dernapp << " [--version,--build,--help] | " << dernapp << " [<filename>]\n";
        astr.Align(4); astr << "--version"; astr.Align(16); astr << "Print Release Info\n";
        astr.Align(4); astr << "--build"; astr.Align(16); astr << "Print Build Info\n";
        astr.Align(4); astr << "--help"; astr.Align(16); astr << "Print This Menu\n";
        astr.Align(4); astr << "<filename>"; astr.Align(16); astr << "The file to execute. Extension: .dern\n";
        astr << "\nFor more help, visit git repository (" << Dern::BuildInfo::Repo << ")\n";

/*        std::cout << Dern::BuildInfo::Name << " Help:\n";
        std::cout << "Usage: dernapp [--version,--help] | dernapp [<filename>]\n";
        std::cout << "\t--version\tPrint Build Version\n";
        std::cout << "\t--help\tPrint This Menu\n";
        std::cout << "\t<filename>\tThe file to execute. Extension: .dern\n";
        std::cout << "\nFor more help, visit git repository (" << Dern::BuildInfo::Repo << ")\n";*/
    }
}