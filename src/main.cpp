#include "pch.h"
#include "program.h"

int main(int argc, char** argv)
{
	if (argc <= 1)
	{
		std::cerr << "No file specified!" << std::endl;
		return 1;
	}

	std::string filepath = argv[1];
	auto prog = Dern::CreateScope<Dern::LProgram>(filepath);
	prog->Run();

	return 0;
}

