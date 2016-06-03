#ifndef FOLDER_PLUGIN_LOADER_HPP
#define FOLDER_PLUGIN_LOADER_HPP

#include <string>
#include <vector>

namespace wgt
{
namespace FolderPluginLoader
{
	bool getPluginsCustomPath( std::vector< std::wstring >& plugins, 
							   const std::wstring& pluginPath );
	bool getPluginsExePath();
}
} // end namespace wgt
#endif // FOLDER_PLUGIN_LOADER_HPP
