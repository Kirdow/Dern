#include "program.h"
#include "cmd.h"
#include "cmdruns.h"

char* s_AppName = nullptr;

int main(int argc, char** argv)
{
	char appName[24];
	s_AppName = appName;

	if (argc <= 1)
	{
		std::cerr << "No file specified!" << std::endl;
		return 1;
	}

	// App Name
	{
		std::string arg = argv[0];
		if (arg.rfind("/") != std::string::npos)
			arg = arg.substr(arg.rfind("/") + 1);

		if (arg.length() > 23)
		{
			std::cerr << "App name too long (> 23)!\n";
			return 1;
		}

		for (size_t i = 0; i < 24; i++)
		{
			if (i >= arg.length())
			{
				appName[i] = '\0';
				continue;
			}
			appName[i] = arg[i];
		}
	}

	// Argument Parsing
	bool active = true;
	std::string filePath = "";
	auto cmdLine = Dern::CmdLine(argc, argv);
	for (auto it = cmdLine.begin() + 1; it != cmdLine.end(); it++)
	{
		auto arg = *it;

		if (Dern::CmdRun::Route(arg, it, active))
			filePath = arg;
	}

	if (active)
	{
		if (std::filesystem::exists(filePath))
		{
			std::string filepath = filePath;
			auto prog = Dern::Ref<Dern::LProgram>::Create(filepath);
			prog->Run();
		}
		else
		{
			std::cerr << "File not found: " << filePath << "\n";
		}
	}

	Dern::RefUtil::ValidateReferences();

	return 0;
}

