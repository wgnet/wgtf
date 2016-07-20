#include "generic_plugin_manager.hpp"
#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/env_context.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_generic_plugin/interfaces/i_command_line_parser.hpp"
#include "core_generic_plugin/interfaces/i_component_context_creator.hpp"
#include "core_generic_plugin/interfaces/i_memory_allocator.hpp"
#include "notify_plugin.hpp"
#include "plugin_context_manager.hpp"

#include "core_common/platform_dbg.hpp"
#include "core_common/platform_env.hpp"
#include "core_common/platform_dll.hpp"
#include "core_common/platform_path.hpp"

#include "core_logging/logging.hpp"

#include <algorithm>
#include <iterator>
#include <cstdint>

#ifdef _WIN32
#include <shlwapi.h>
#endif // _WIN32

namespace wgt
{
//==============================================================================
GenericPluginManager::GenericPluginManager(bool applyDebugPostfix_)
	: contextManager_( new PluginContextManager() )
	, applyDebugPostfix(applyDebugPostfix_)
{
}


//==============================================================================
GenericPluginManager::~GenericPluginManager()
{
	// uninitialise in the reverse order. yes, we need a copy here.
	PluginList plugins;
	for (auto it = plugins_.crbegin(); it != plugins_.crend(); ++it)
		plugins.push_back( it->second );
	unloadPlugins( plugins );
}


//==============================================================================
void GenericPluginManager::loadPlugins(
	const PluginNameList& plugins )
{
	PluginList plgs;
	std::transform(
		std::begin( plugins ),
		std::end( plugins ),
		std::back_inserter( plgs ),
		std::bind(
			&GenericPluginManager::loadPlugin, this, std::placeholders::_1) );

	notifyPlugins( plgs,
		NotifyPlugin ( *this, GenericPluginLoadState::Create ) );

	notifyPlugins( plgs, NotifyPluginPostLoad( *this ) );

	notifyPlugins( plgs,
		NotifyPlugin ( *this, GenericPluginLoadState::Initialise ) );
}


//==============================================================================
void GenericPluginManager::unloadPlugins(
	const PluginNameList& plugins )
{
	PluginList plgs;
	for ( auto & filename : plugins )
	{
		auto it = std::find_if( plugins_.begin(), plugins_.end(),
			[&](PluginMap::value_type& p) { return filename == p.first; } );

		if (it != plugins_.end())
		{
			plgs.push_back( it->second );
		}
	}

	unloadPlugins( plgs );
}


//==============================================================================
void GenericPluginManager::unloadPlugins( const PluginList& plugins )
{
	if (plugins.empty())
	{
		return;
	}

	for( int state = Finalise; state < Destroy; ++state)
	{
		notifyPlugins( plugins,
			NotifyPlugin ( *this, ( GenericPluginLoadState ) state ) );
	}

	// Notify plugins of destroy - Matches Create notification
	notifyPlugins( plugins,  NotifyPlugin ( *this, Destroy ) );

	// Do in reverse order of load
	std::for_each( std::begin( plugins ), std::end( plugins ), std::bind(
		&GenericPluginManager::unloadContext, this, std::placeholders::_1 ) );

	// Calls FreeLibrary - matches loadPlugin() LoadLibraryW
	std::for_each( std::begin( plugins ), std::end( plugins ), std::bind(
		&GenericPluginManager::unloadPlugin, this, std::placeholders::_1 ) );

	auto it = memoryContext_.begin();
	for( ; it != memoryContext_.end(); ++it )
	{
		::OutputDebugString( it->first.c_str() );
		::OutputDebugString( L"\n" );
		delete it->second;
	}

	memoryContext_.clear();
}


//==============================================================================
void GenericPluginManager::notifyPlugins(
	const PluginList& plugins, NotifyFunction func )
{
	std::for_each( std::begin( plugins ), std::end( plugins ), func );
}

//==============================================================================
HMODULE GenericPluginManager::loadPlugin( const std::wstring & filename )
{
	std::string errorMsg;
	auto processedFileName = processPluginFilename( filename );

	setEnvContext( contextManager_->createContext( filename ) );
	HMODULE hPlugin = ::LoadLibraryW( processedFileName.c_str() );
	// Must get last error before doing anything else
	const bool hadError = FormatLastErrorMessage( errorMsg );
	setEnvContext( nullptr );

	if (hPlugin != nullptr)
	{
		plugins_.push_back( PluginMap::value_type(filename, hPlugin) );
	}
	else
	{
		contextManager_->destroyContext( filename );

		NGT_ERROR_MSG( "Could not load plugin %S (from %S): %s\n",
			filename.c_str(),
			processedFileName.c_str(),
			hadError ? errorMsg.c_str() : "Unknown error" );

#if defined( _DEBUG )
		// Fail automated tests
		const auto pCommandLine = this->queryInterface< ICommandLineParser >();
		const auto requireAllSpecifiedPlugins = (pCommandLine == nullptr) ?
			true :
			pCommandLine->getFlag( "-unattended" );

		if (requireAllSpecifiedPlugins)
		{
			assert( false && "Could not load plugin" );
		}
#endif // defined( _DEBUG )
	}
	return hPlugin;
}

//==============================================================================
GenericPluginManager::PluginMap::iterator GenericPluginManager::findPlugin(HMODULE hPlugin)
{
	return std::find_if( plugins_.begin(), plugins_.end(),
		[&](PluginMap::value_type& p) { return hPlugin == p.second; } );
}

//==============================================================================
void GenericPluginManager::unloadContext( HMODULE hPlugin )
{
	PluginMap::iterator it = findPlugin(hPlugin);

	if ( it == std::end( plugins_ ) )
	{
		return;
	}

	IComponentContext * contextManager =
		contextManager_->getContext( it->first );
	IMemoryAllocator * memoryAllocator =
		contextManager->queryInterface< IMemoryAllocator >();
	contextManager_->destroyContext( it->first );
	memoryContext_.insert( std::make_pair( it->first, memoryAllocator ) );
}

//==============================================================================
bool GenericPluginManager::unloadPlugin( HMODULE hPlugin )
{
	if (!hPlugin)
	{
		return false;
	}

	PluginMap::iterator it = findPlugin(hPlugin);
	assert( it != std::end( plugins_ ) );

	::FreeLibrary( hPlugin );
	plugins_.erase ( it );

	return true;
}


//==============================================================================
IPluginContextManager & GenericPluginManager::getContextManager() const
{
	return *contextManager_.get();
}


//==============================================================================
void * GenericPluginManager::queryInterface( const char * name ) const
{
	return getContextManager().getGlobalContext()->queryInterface(
		name );
}


//==============================================================================
std::wstring GenericPluginManager::processPluginFilename(const std::wstring& filename)
{
	// PathCanonicalize does not convert '/' to '\\'
	wchar_t normalisedPath[MAX_PATH];
	std::copy(filename.c_str(), filename.c_str() + filename.size(), normalisedPath);
	normalisedPath[filename.size()] = L'\0';

#ifdef _WIN32
	std::replace(normalisedPath, normalisedPath + filename.size(), L'/', L'\\');
#elif __APPLE__
	std::replace(normalisedPath, normalisedPath + filename.size(), L'\\', L'/' );
	wchar_t file[MAX_PATH];
	PathFileName(file, normalisedPath);
	PathRemoveFileSpec(normalisedPath);
	PathAppend(normalisedPath, L"lib");
	PathAppend(normalisedPath, file);
#endif

	wchar_t temp[MAX_PATH];

	if (PathIsRelative(normalisedPath))
	{
#ifdef __APPLE__
		wcpcpy(temp, normalisedPath);
#elif _WIN32
		wchar_t exePath[MAX_PATH];
		if (contextManager_->getExecutablePath())
		{
			mbstowcs(exePath, contextManager_->getExecutablePath(), strlen(contextManager_->getExecutablePath()) + 1);
		}
		else
		{
			GetModuleFileName(nullptr, exePath, MAX_PATH);
			PathRemoveFileSpec(exePath);
		}
		PathAppend(exePath, normalisedPath);
		PathCanonicalize(temp, exePath);
#endif
	}
	else
	{
		PathCanonicalize(temp, normalisedPath);
	}


	PathRemoveExtension(temp);

#ifdef _DEBUG
	if (applyDebugPostfix)
	{
		const size_t len = ::wcsnlen(temp, MAX_PATH);
		if (::wcsncmp(temp + len - 2, L"_d", 2) != 0)
		{
			wcscat(temp, L"_d");
		}
	}
#endif

	AddDllExtension(temp);

	return  temp;
}
} // end namespace wgt
