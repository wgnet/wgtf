#include "wg_memory/memory_overrides.hpp"

#include "core_generic_plugin/interfaces/i_plugin_context_manager.hpp"
#include "core_generic_plugin_manager/generic_plugin_manager.hpp"
#include "core_generic_plugin_manager/folder_plugin_loader.hpp"
#include "core_generic_plugin_manager/config_plugin_loader.hpp"
#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_logging/logging.hpp"
#include "memory_plugin_context_creator.hpp"
#include "command_line_parser.hpp"

#include "core_common/platform_path.hpp"
#include <locale>
#include <codecvt>
#include <csignal>

#ifdef _WIN32
#include <stdlib.h>
#endif // _WIN32

namespace wgt
{
namespace
{
	
#ifdef _WIN32
static const wchar_t* const pluginsFolder = L"plugins\\";
#elif __APPLE__
static const wchar_t* const pluginsFolder = L"../Resources/plugins/";
#endif // __APPLE__

bool getPlugins(std::vector<std::wstring>& plugins, const wchar_t* configFile)
{
	wchar_t path[MAX_PATH];
	::GetModuleFileNameW( NULL, path, MAX_PATH );
	::PathRemoveFileSpecW( path );

	if ((configFile == NULL) || (wcscmp( configFile, L"" ) == 0))
	{
		::PathAppendW(path, pluginsFolder);

		return
			ConfigPluginLoader::getPlugins(
			plugins, std::wstring( path ) +
#ifdef __APPLE__
			L"plugins_ui_mac.txt" )
#else
			L"plugins_ui.txt" )
#endif 
			||
			FolderPluginLoader::getPluginsCustomPath( plugins, path );
	}
	else
	{
		::PathAppendW(path, pluginsFolder);
		::PathAppendW( path, configFile );
		return ConfigPluginLoader::getPlugins( plugins, path );
	}
}
}

#ifdef _WIN32
void purecallHandler(void)
{
	NGT_ERROR_MSG("Pure virtual function call\n");
	_exit(3);
}

void signalHandler(int signal)
{
	NGT_ERROR_MSG("Signal %d\n", signal);
	_exit(3);
}
#endif // _WIN32

int Main(int argc, char **argv)
{
	CommandLineParser * clp = new CommandLineParser( argc, argv );
#ifdef _WIN32
	if (clp->getFlag( "-unattended" ))
	{
		_set_purecall_handler(purecallHandler);

		typedef void (*SignalHandlerPointer)(int);
		SignalHandlerPointer previousHandler;
		previousHandler = signal(SIGINT, signalHandler);
		previousHandler = signal(SIGILL, signalHandler);
		previousHandler = signal(SIGFPE, signalHandler);
		previousHandler = signal(SIGSEGV, signalHandler);
		previousHandler = signal(SIGTERM, signalHandler);
		previousHandler = signal(SIGBREAK, signalHandler);
		previousHandler = signal(SIGABRT, signalHandler);
		previousHandler = signal(SIGABRT_COMPAT, signalHandler);

		_set_error_mode(_OUT_TO_STDERR);
		_set_abort_behavior( 0, _WRITE_ABORT_MSG);
	}

	// Replaces assert dialogs with a log to the console.
	// Which is useful if the GUI gets broken and the assert dialogs don't work
	// or you want to ignore asserts.
	// Put a breakpoint on OutputDebugString to break on asserts.
	if (clp->getFlag( "--log-asserts" ))
	{
		_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_DEBUG );
	}
#endif // _WIN32
	auto allocatorDebugOutput = clp->getFlag( "--allocatorDebugOutput" );
	auto allocatorStackTraces = clp->getFlag( "--allocatorStackTraces" );
	const auto allocatorLeakDetection = clp->getFlag("--allocatorLeakDetection");
	NGTAllocator::enableDebugOutput( allocatorDebugOutput );
	NGTAllocator::enableStackTraces( allocatorStackTraces );
	NGTAllocator::enableLeakDetection(allocatorLeakDetection);

	std::wstring config;
#ifdef _CONFIG_FILE_NAME
	config = cmake_gen::configFileName;
#else
	config = clp->getParamStrW("--config");
#endif // _CONFIG_FILE_NAME

	std::vector< std::wstring > plugins;
	if (!getPlugins( plugins, config.c_str() ) || plugins.empty())
	{
		return 2; // failed to find any plugins!
	}

	int result = 1;

	{
		GenericPluginManager pluginManager;
		IPluginContextManager & contextManager =
			pluginManager.getContextManager();

		auto globalContext = contextManager.getGlobalContext();
		globalContext->registerInterface( new MemoryPluginContextCreator );
		globalContext->registerInterface( clp );
		pluginManager.loadPlugins(plugins);

		IApplication* application =
			contextManager.getGlobalContext()->queryInterface< IApplication >();
		if (application != NULL)
		{
			result = application->startApplication();
		}
	}
	return result;
}
} // end namespace wgt

#ifdef _WIN32
int STDMETHODCALLTYPE WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
			 LPSTR lpCmdLine, int nShowCmd )
{
	int argc = __argc;
	char** argv = __argv;
#endif // _WIN32
	
#ifdef __APPLE__
int main(int argc, char **argv, char **envp, char **apple)
{
#endif // __APPLE__

	return wgt::Main( argc, argv );
}
