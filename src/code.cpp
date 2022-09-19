#include "pch.h"
#include "code.h"

namespace Dern
{
	void Code::Load()
	{
		std::string result;
		std::ifstream in(m_Path, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else
		{
			std::cerr << "FAILED TO OPEN CODE FILE '" << m_Path << "'" << std::endl;
		}

		m_Data = result;
	}
}


