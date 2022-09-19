#include "pch.h"
#include "program.h"
#include "registry.h"

int main(int argc, char** argv)
{
	Dern::Registry reg;
	std::cout << "Default State!\n";
	std::cout << "Has Int[a]: " << reg.HasEntry<int>("a") << "\n";
	std::cout << "Has String[a]: " << reg.HasEntry<std::string>("a") << "\n";
	std::cout << "Get Int[a]: " << reg.GetEntry<int>("a") << "\n";
	std::cout << "Get String[a]: \"" << reg.GetEntry<std::string>("a") << "\"\n";

	reg.SetEntry<int>("a", 5);
	std::cout << "\nSet Int[a]: 5\n";
	std::cout << "Has Int[a]: " << reg.HasEntry<int>("a") << "\n";
	std::cout << "Has String[a]: " << reg.HasEntry<std::string>("a") << "\n";
	std::cout << "Get Int[a]: " << reg.GetEntry<int>("a") << "\n";
	std::cout << "Get String[a]: \"" << reg.GetEntry<std::string>("a") << "\"\n";

	reg.SetEntry<std::string>("a", "hello");
	std::cout << "\nSet String[a]: \"hello\"\n";
	std::cout << "Has Int[a]: " << reg.HasEntry<int>("a") << "\n";
	std::cout << "Has String[a]: " << reg.HasEntry<std::string>("a") << "\n";
	std::cout << "Get Int[a]: " << reg.GetEntry<int>("a") << "\n";
	std::cout << "Get String[a]: \"" << reg.GetEntry<std::string>("a") << "\"\n";

	reg.UnsetEntry("a");
	std::cout << "\nUnset[a]\n";
	std::cout << "Has Int[a]: " << reg.HasEntry<int>("a") << "\n";
	std::cout << "Has String[a]: " << reg.HasEntry<std::string>("a") << "\n";
	std::cout << "Get Int[a]: " << reg.GetEntry<int>("a") << "\n";
	std::cout << "Get String[a]: \"" << reg.GetEntry<std::string>("a") << "\"\n";
	

	if (true) return 0;
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

