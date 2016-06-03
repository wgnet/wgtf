#include "pch.hpp"

#include "core_common/ngt_windows.hpp"
#include "py_script_object.hpp"

namespace wgt
{
namespace PyScript
{

/* static */ bool ScriptModule::moduleExists( const char * name )
{
	assert( name );
	
	char buf[ MAX_PATH ];

	FILE * fp = NULL;
	PyObject * pLoader = NULL;

	bool result = (_PyImport_FindModule( name, NULL, buf, sizeof( buf ),
		&fp, &pLoader ) != NULL);

	if (fp)
	{
		fclose( fp );
	}

	Py_XDECREF( pLoader );

	Script::clearError();

	return result;
}


} // namespace PyScript

// py_script_object.cpp
} // end namespace wgt
