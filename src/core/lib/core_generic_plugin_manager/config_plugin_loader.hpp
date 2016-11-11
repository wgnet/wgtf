#ifndef CONFIG_PLUGIN_LOADER_HPP
#define CONFIG_PLUGIN_LOADER_HPP

#include <string>
#include <vector>

namespace wgt
{
namespace ConfigPluginLoader
{
bool getPlugins(std::vector<std::wstring>& plugins, const std::wstring& configFileName);
}
} // end namespace wgt
#endif // CONFIG_PLUGIN_LOADER_HPP
