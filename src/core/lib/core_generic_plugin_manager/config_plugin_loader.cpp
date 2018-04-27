#include "config_plugin_loader.hpp"

#include <fstream>
#include <iterator>
#include <codecvt>
#include <algorithm>

namespace wgt
{
namespace ConfigPluginLoader
{
typedef std::istream_iterator<std::wstring, wchar_t, std::char_traits<wchar_t>> wistream_iterator;

bool getPlugins(std::vector<std::wstring>& plugins, const std::wstring& configFileName)
{
#ifdef _WIN32
	std::wifstream inputFile(configFileName.c_str());
#endif // _WIN32

#ifdef __APPLE__
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	std::wifstream inputFile(conv.to_bytes(configFileName).c_str());
#endif // __APPLE__

	if (inputFile.good())
	{
		std::wstring line;
		while (getline(inputFile, line))
		{
			if (line.size() == 0 || line[0] == '#')
			{
				continue;
			}

			plugins.emplace_back(line);
		}

		return true;
	}

	return false;
}
}
} // end namespace wgt
