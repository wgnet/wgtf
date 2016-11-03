#include "pch.hpp"

#include "core_logging/logging.hpp"
#include "private/python_importer.hpp"
#include "scripting_engine.hpp"
#include "defined_instance.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_generic_plugin/interfaces/i_command_line_parser.hpp"
#include "wg_pyscript/py_script_object.hpp"
#include "wg_pyscript/py_script_output_writer.hpp"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <cwchar>


namespace wgt
{
namespace Python27ScriptingEngine_Detail
{


/**
 *	Append path to source *.py module, that is not packed in the binary.
 *	@param path relative path to module from the exe.
 *		e.g. "../plg_example/scripts/my_module"
 *	@return false if path was already added.
 */
bool appendPath( const wchar_t * path )
{
	if ((path == nullptr) || (wcslen( path ) == 0))
	{
		// Already added
		return true;
	}

	// Normalize path
	// Replace '/' with '\\' on Windows
	// Because Python always uses the platform's native format
#if defined( WIN32 )
	std::wstring tmpPath( path );
	std::replace( std::begin( tmpPath ), std::end( tmpPath ), L'/', L'\\' );
	const auto normalizedPath = tmpPath.c_str();
#else // defined( WIN32 )
	const auto normalizedPath = path;
#endif // defined( WIN32 )

	PyScript::ScriptObject testPathObject =
		PyScript::ScriptObject::createFrom( normalizedPath );

	PyObject* pySysPaths = PySys_GetObject( "path" );
	if (pySysPaths == nullptr)
	{
		NGT_ERROR_MSG( "Unable to get sys.path\n" );
		return false;
	}

	PyScript::ScriptList sysPaths( pySysPaths );

	// Check for duplicates
	for (PyScript::ScriptList::size_type i = 0; i < sysPaths.size(); ++i)
	{
		auto item = sysPaths.getItem( i );
		if (item.compareTo( testPathObject, PyScript::ScriptErrorPrint() ) == 0)
		{
			// Already added
			return true;
		}
	}

	// Add new path
	sysPaths.append( testPathObject );

	const int result = PySys_SetObject( "path", pySysPaths );
	if (result != 0)
	{
		NGT_ERROR_MSG( "Unable to assign sys.path\n" );
		return false;
	}

	return true;
}


ObjectHandle import( IComponentContext & context, const char * name )
{
	if (!Py_IsInitialized())
	{
		return nullptr;
	}

	if ((name == nullptr) || (strlen( name ) == 0))
	{
		return nullptr;
	}

	PyScript::ScriptModule module = PyScript::ScriptModule::import( name,
		PyScript::ScriptErrorPrint( "Unable to import\n" ) );

	if (!module.exists())
	{
		return nullptr;
	}

	ObjectHandle parentHandle;
	const char * childPath = "";
	return ReflectedPython::DefinedInstance::findOrCreate( context, module, parentHandle, childPath );
}


void initDebugger( const DIRef< ICommandLineParser > & commandLine )
{
	// generic_app_d --pydev --host localhost --stdoutToServer --stderrToServer --port 1234 --suspend
	const auto usePydev = commandLine->getFlag( "--pydev" );
	if (!usePydev)
	{
		return;
	}

	// Requires pydevd module added to the user's PYTHONPATH
	auto pyDevModule = PyScript::ScriptModule::import( "pydevd",
		PyScript::ScriptErrorRetain() );
	if (!pyDevModule.exists() || PyScript::Script::hasError())
	{
		PyScript::Script::printError();
		return;
	}


	// pydevd.settrace('localhost', True, True, 1234, True)
	const char * defaultHost = "localhost";
	const bool defaultStdoutToServer = false;
	const bool defaultStderrToServer = false;
	const int defaultPort = 1234;
	const bool defaultSuspend = false;

	auto host = defaultHost;
	auto stdoutToServer = defaultStdoutToServer;
	auto stderrToServer = defaultStderrToServer;
	auto port = defaultPort;
	auto suspend = defaultSuspend;
	if (commandLine != nullptr)
	{
		const auto hostString = commandLine->getParam( "--host" );
		if (hostString != nullptr)
		{
			host = hostString;
		}

		stdoutToServer = commandLine->getFlag( "--stdoutToServer" );
		stderrToServer = commandLine->getFlag( "--stderrToServer" );

		const auto portString = commandLine->getParam( "--port" );
		if (portString != nullptr)
		{
			port = atoi( portString );
		}

		suspend = commandLine->getFlag( "--suspend" );
	}
	
	auto args = PyScript::ScriptArgs::create( host,
		stdoutToServer,
		stderrToServer,
		port,
		suspend );
	pyDevModule.callMethod( "settrace",
		args,
		PyScript::ScriptErrorPrint() );
}

} // end namespace Python27ScriptingEngine_Detail

#if ENABLE_DEPLOY_PYTHON_SUPPORT
std::unique_ptr< PythonDependencies > g_Deps;
#endif // ENABLE_DEPLOY_PYTHON_SUPPORT

Python27ScriptingEngine::Python27ScriptingEngine( IComponentContext& context )
	: context_( context )
	, commandLine_( context )
{
#if ENABLE_DEPLOY_PYTHON_SUPPORT
	assert( g_Deps == nullptr );
	g_Deps.reset( new PythonDependencies( context_ ) );
#endif // ENABLE_DEPLOY_PYTHON_SUPPORT
}


Python27ScriptingEngine::~Python27ScriptingEngine()
{
#if ENABLE_DEPLOY_PYTHON_SUPPORT
	g_Deps.reset();
#endif // ENABLE_DEPLOY_PYTHON_SUPPORT
}


bool Python27ScriptingEngine::init()
{
	// Warn if tab and spaces are mixed in indentation.
	Py_TabcheckFlag = 1;
	// Disable importing Lib/site.py on startup.
	Py_NoSiteFlag = 1;
	// Enable debug output
	// Requires the scriptoutputwriter output hook from stdout/stderr
	//Py_VerboseFlag = 2;
	// Use environment variables
	Py_IgnoreEnvironmentFlag = 0;

	// Tell the interpreter the value of the argv[0]
	// This will add the exe's folder to the interpreter's path
	// Must be before Py_Initialize()
	if ((commandLine_ != nullptr) && (commandLine_->argc() > 0))
	{
		const auto exePath = commandLine_->argv()[0];
		Py_SetProgramName( exePath );
	}

	// Initialize logging as a standard module
	// Must be before Py_Initialize()
	PyImport_AppendInittab( "scriptoutputwriter",
		PyScript::PyInit_ScriptOutputWriter );

	Py_Initialize();

	// Import the logging module
	// Must be after Py_Initialize()
	auto outputModule = PyScript::ScriptModule::import( "scriptoutputwriter",
		PyScript::ScriptErrorPrint() );
	assert( outputModule.exists() );

	// Allow import from supported system modules
	const size_t BUFFER_SIZE = 256;
	wchar_t pythonSourcePath[ BUFFER_SIZE ];
	const auto result = swprintf(pythonSourcePath,
	                             BUFFER_SIZE,
	                             L"../../../src/core/third_party/python/python-%d.%d.%d/Lib",
	                             PY_MAJOR_VERSION,
	                             PY_MINOR_VERSION,
	                             PY_MICRO_VERSION);
	assert( result > 0 );
	if (!this->appendSourcePath( pythonSourcePath ))
	{
		NGT_ERROR_MSG( "Failed to append path to system modules\n" );
		return false;
	}
	// TODO NGT-2836 Work-around
	//if (!this->appendBinPath( L":/Scripts" ))
	if (!this->appendBinPath( L"./scripts/common/Lib" ))
	{
		NGT_ERROR_MSG( "Failed to append path to system modules\n" );
		return false;
	}

#if ENABLE_DEPLOY_PYTHON_SUPPORT
	auto resourceSystem = getPythonDependency<IResourceSystem>();
	assert( (resourceSystem != nullptr) && "IResourceSystem required to import" );
	if (resourceSystem != nullptr)
	{
		WGTPythonImport::init();
	}
#endif // ENABLE_DEPLOY_PYTHON_SUPPORT

	// Import the (optional) debugging module
	Python27ScriptingEngine_Detail::initDebugger(commandLine_);

	return true;
}


void Python27ScriptingEngine::fini()
{
	// Must not use any PyObjects after this point
	Py_Finalize();
}


bool Python27ScriptingEngine::appendSourcePath( const wchar_t * path )
{
	// Enabled in deploy mode to support pydev debugging
	return Python27ScriptingEngine_Detail::appendPath(path);
}


bool Python27ScriptingEngine::appendBinPath( const wchar_t * path )
{
#if ENABLE_DEPLOY_PYTHON_SUPPORT
	return wgt::WGTPythonImport::appendPath( path );
#else // ENABLE_DEPLOY_PYTHON_SUPPORT
	return true; // Ignore
#endif // ENABLE_DEPLOY_PYTHON_SUPPORT
}


ObjectHandle Python27ScriptingEngine::import( const char * moduleName )
{
	return Python27ScriptingEngine_Detail::import(context_, moduleName);
}


bool Python27ScriptingEngine::checkErrors()
{
	if (PyScript::Script::hasError())
	{
		PyScript::Script::printError();
		PyScript::Script::clearError();
		return true;
	}

	return false;
}
} // end namespace wgt
