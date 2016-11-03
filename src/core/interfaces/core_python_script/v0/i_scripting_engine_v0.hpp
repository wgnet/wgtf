#pragma once
#ifndef I_V0_PYTHON_SCRIPTING_ENGINE_V0_HPP
#define I_V0_PYTHON_SCRIPTING_ENGINE_V0_HPP

#include "core_dependency_system/i_interface.hpp"

#include <memory>

namespace wgt
{
class ObjectHandle;

DECLARE_INTERFACE_BEGIN( IPythonScriptingEngine, 0, 0 )

/**
	 *	Add a path the the "sys.path" list to search when importing modules.
	 *	
	 *	Ideally two paths are appended:
	 *		1. to the source *.py files used for development.
	 *		2. to the deployed *.pyc files distributed with the exe.
	 *	
	 *	Use this function to set the path to source *.py files as described in 1.
	 *	
	 *	@pre interpreter must be initialized.
	 *	@note appendSourcePath() should be used before appendBinPath() to allow
	 *		pydev debuggers to find the source.
	 *	
	 *	@param path to be added to the "sys.path" list.
	 *		Does not check if the path is valid.
	 *		Calling code should check if the path is valid before appending.
	 *		A null or empty path to not append anything.
	 *	
	 *	@return true on success.
	 */
virtual bool appendSourcePath(const wchar_t* path) = 0;

/**
	 *	@see appendSourcePath()
	 *	Use this function to set the path to deployed *.pyc files as described in 2.
	 */
    virtual bool appendBinPath( const wchar_t * path ) = 0;

	/**
	 *	Import a Python module using the search paths in "sys.path".
	 *	
	 *	@pre interpreter must be initialized.
	 *	
	 *	@param name the name of the module to import.
	 *		e.g. import( "test" ) will search for "test.py".
	 *		A null or empty path to not import anything.
	 *		Always use forward-slashes in the path '/'.
	 *	
	 *	@return module that has been imported or nullptr on failure.
	 */
	virtual ObjectHandle import( const char * moduleName ) = 0;

	/**
	 *	Check for any errors during script execution and print them.
	 *	
	 *	@pre interpreter must be initialized.
	 *	
	 *	@return true if errors occurred.
	 */
	virtual bool checkErrors() = 0;

DECLARE_INTERFACE_END()
} // end namespace wgt
#endif // I_V0_PYTHON_SCRIPTING_ENGINE_V0_HPP
