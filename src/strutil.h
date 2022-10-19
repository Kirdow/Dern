#pragma once

#include <string>

namespace StrUtil
{
	bool Contains(const std::string_view& haystack, const std::string_view& needle);

	int Split(const std::string& str, const std::string& delim, std::vector<std::string>& v);
	int SplitSymbols(const std::string& str, const std::string& syms, std::vector<std::string>& v);

	bool StartsWith(const std::string& str, const std::string& needle);
	bool EndsWith(const std::string& str, const std::string& needle);

	const std::string& GetDirectorySeparator();

	namespace Self
	{
		inline static void LTrim(std::string& s)
		{
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch)
			{
				return !std::isspace(ch);
			}));
		}
		
		inline static void RTrim(std::string& s)
		{
			s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
			{
				return !std::isspace(ch);
			}).base(), s.end());
		}

		inline static void Trim(std::string& s)
		{
			LTrim(s);
			RTrim(s);
		}
	}

	namespace Copy
	{
		inline static std::string LTrim(std::string s)
		{
			StrUtil::Self::LTrim(s);
			return s;
		}

		inline static std::string RTrim(std::string s)
		{
			StrUtil::Self::RTrim(s);
			return s;
		}

		inline static std::string Trim(std::string s)
		{
			StrUtil::Self::Trim(s);
			return s;
		}
	}

}
