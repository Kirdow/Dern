#pragma once

#include "strutil.h"
#include <string>
#include <ostream>

namespace Dern
{
	class Code
	{
	public:
		Code(const std::string& path) : m_Path(path)
		{
			Load();
		}

		void Reload() { m_Data = ""; Load(); }

		char operator[](int n) const { return m_Data[n]; }
		size_t Length() const { return m_Data.length(); }
		
		const std::string& Raw() const { return m_Data; }
	private:
		void Load();
	private:
		std::string m_Path;
		std::string m_Data;
	};
}

inline std::ostream& operator<<(std::ostream& ostr, const Dern::Code& code)
{
	std::string s = code.Raw();
	StrUtil::Self::RTrim(s);
	ostr << s << '\n';

	return ostr;
}

