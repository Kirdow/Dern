#pragma once

#include "code.h"

namespace Dern
{
	class Registry;
	class LProgram
	{
	public:
		LProgram(const std::string& filePath);
		~LProgram();

		void Run();
	private:
		Scope<Code> m_Code;
		Ref<Registry> m_Registry;
	};
}
