#include "folder_plugin_loader.hpp"

#ifdef _WIN32
#include <shlwapi.h>
#endif

#ifdef __APPLE__
#include <stdlib.h>
#include <dirent.h>
#include <dlfcn.h>
#include <libgen.h>
#include <codecvt>
#include <locale>
#include "core_logging/logging.hpp"
#endif

namespace wgt
{
#ifdef _WIN32
namespace
{
	bool getPluginsInternal( std::vector< std::wstring > & plugins,
							const std::wstring & pluginPath )
	{
		WIN32_FIND_DATA find_data;
		HANDLE hFile = ::FindFirstFileW(
			( pluginPath + L"*.dll" ).c_str(), &find_data );
		while (hFile != INVALID_HANDLE_VALUE)
		{
			plugins.push_back( pluginPath + find_data.cFileName );

			// go for the next DLL
			if (!::FindNextFile( hFile, &find_data ))
			{
				break;
			}
		}

		return true;
	}
}

namespace FolderPluginLoader
{

bool getPluginsCustomPath( std::vector< std::wstring >& plugins,
						   const std::wstring& pluginPath )
{
	WCHAR fullPath[MAX_PATH];
	::PathCanonicalizeW( fullPath, pluginPath.c_str() );

	return getPluginsInternal( plugins, fullPath );
}

bool loadPluginsExePath( std::vector< std::wstring >& plugins )
{
	WCHAR exePath[MAX_PATH];
	::GetModuleFileNameW( NULL, exePath, MAX_PATH );
	::PathRemoveFileSpecW( exePath );

	return getPluginsInternal( plugins, exePath );
}

}
#endif // _WIN32

#ifdef __APPLE__

namespace
{
	bool getPluginsInternal( std::vector< std::wstring > & plugins, const std::string & pluginPath )
	{
		if (DIR* dir = opendir( pluginPath.c_str() ))
		{
			errno = 0;
			while (dirent* f = readdir(dir))
			{
				if (f->d_name[0] == '.' )
					continue;

				if (strstr( f->d_name, ".dylib" ))
				{
					std::wstring_convert< std::codecvt_utf8<wchar_t> > conv;
					plugins.push_back( conv.from_bytes( pluginPath + f->d_name ) );
				}
			}
			closedir(dir);
		}
		return true;
	}
}

namespace FolderPluginLoader
{
	bool getPluginsCustomPath( std::vector< std::wstring >& plugins, const std::wstring& pluginPath )
	{
		char fullPath[PATH_MAX];
		std::wstring_convert< std::codecvt_utf8<wchar_t> > conv;
		return realpath( conv.to_bytes(pluginPath).c_str(), fullPath ) && getPluginsInternal( plugins, fullPath );
	}

	bool loadPluginsExePath( std::vector< std::wstring >& plugins )
	{
		Dl_info info;
		if (!dladdr( reinterpret_cast<void*>(loadPluginsExePath), &info ))
		{
			NGT_ERROR_MSG( "Folder plugin loader: failed to get current module file name%s", "\n" );
			return false;
		}
		char path[PATH_MAX];
		strcpy(path, info.dli_fname);
		std::string exePath = dirname( path );
		return getPluginsInternal( plugins, exePath );
	}
}
#endif // __APPLE__
} // end namespace wgt
