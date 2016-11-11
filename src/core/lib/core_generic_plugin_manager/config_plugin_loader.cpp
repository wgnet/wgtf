#include "config_plugin_loader.hpp"

#include <fstream>
#include <iterator>
#include <codecvt>

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
		std::vector<std::wstring> v((wistream_iterator(inputFile)), wistream_iterator());
		plugins.swap(v);
		return true;
	}

	return false;
}
}
} // end namespace wgt
