#include "pch.hpp"

#include "core_logging/logging.hpp"

#include "scripting_engine.hpp"
#include "defined_instance.hpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"

#include "wg_pyscript/py_script_object.hpp"
#include "wg_pyscript/py_script_output_writer.hpp"

#include <algorithm>
#include <cstring>
#include <cwchar>


namespace wgt
{
namespace
{

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


} // namespace


Python27ScriptingEngine::Python27ScriptingEngine( IComponentContext& context )
	: context_( context )
{
}


Python27ScriptingEngine::~Python27ScriptingEngine()
{
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

	// Initialize logging as a standard module
	// Must be before Py_Initialize()
	PyImport_AppendInittab( "scriptoutputwriter",
		PyScript::PyInit_ScriptOutputWriter );

	Py_Initialize();
	
	// Import the logging module
	// Must be after Py_Initialize()
	PyImport_ImportModule( "scriptoutputwriter" );

	// Allow import from supported system modules
	const size_t BUFFER_SIZE = 256;
	wchar_t pythonSourcePath[ BUFFER_SIZE ];
	const auto result = swprintf( pythonSourcePath,
		BUFFER_SIZE,
		L"../../../src/core/third_party/python/Python-%d.%d.%d/Lib",
		PY_MAJOR_VERSION,
		PY_MINOR_VERSION,
		PY_MICRO_VERSION );
	assert( result > 0 );
	if (!this->appendSourcePath( pythonSourcePath ))
	{
		NGT_ERROR_MSG( "Failed to append path to system modules\n" );
		return false;
	}
	if (!this->appendBinPath( L"./scripts/common/Lib" ))
	{
		NGT_ERROR_MSG( "Failed to append path to system modules\n" );
		return false;
	}

	return true;
}


void Python27ScriptingEngine::fini()
{
	// Must not use any PyObjects after this point
	Py_Finalize();
}


bool Python27ScriptingEngine::appendSourcePath( const wchar_t * path )
{
	return wgt::appendPath( path );
}


bool Python27ScriptingEngine::appendBinPath( const wchar_t * path )
{
	return wgt::appendPath( path );
}


ObjectHandle Python27ScriptingEngine::import( const char * moduleName )
{
	return wgt::import( context_, moduleName );
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
