
namespace StrUtil
{
	bool Contains(const std::string_view& haystack, const std::string_view& needle)
	{
		return haystack.find(needle) != std::string_view::npos;
	}

	int Split(const std::string& str, const std::string& delim, std::vector<std::string>& v)
	{
		using sizeint = std::string::size_type;
		int count = 0;
		sizeint index = 0;
		sizeint lastIndex = 0;
		while ((index = str.find(delim, lastIndex)) != std::string::npos)
		{
			v.push_back(str.substr(lastIndex, index - lastIndex));
			lastIndex = index + delim.length();
			++count;
		}

		v.push_back(str.substr(lastIndex));

		return count;
	}

	int SplitSymbols(const std::string& str, const std::string& syms, std::vector<std::string>& v)
	{
		std::unordered_set<unsigned char> cmap;
		for (auto ch : syms)
		{
			cmap.insert(static_cast<unsigned char>(ch));
		}

		auto findSymsFn = [&cmap](unsigned char ch)
		{
			return cmap.find(ch) != cmap.end();
		};

		int count = 0;
		auto index = str.begin();
		auto lastIndex = index;
		while ((index = std::find_if(lastIndex, str.end(), findSymsFn)) != str.end())
		{
			v.push_back(std::string(lastIndex, index));
			lastIndex = index + 1;
			++count;
		}

		v.push_back(std::string(lastIndex, str.end()));

		return count;
	}

	bool StartsWith(const std::string& str, const std::string& needle)
	{
		return (str.find(needle) == 0);
	}

	bool EndsWith(const std::string& str, const std::string& needle)
	{
		return (str.find(needle) == str.length() - needle.length());
	}
}
