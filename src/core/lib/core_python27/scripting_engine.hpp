#pragma once
#ifndef PYTHON_SCRIPTING_ENGINE_HPP
#define PYTHON_SCRIPTING_ENGINE_HPP

#include "interfaces/core_python_script/i_scripting_engine.hpp"
#include "core_script/type_converter_queue.hpp"


namespace wgt
{
class IComponentContext;
class ObjectHandle;


/**
 *	Provides the public interface to Python 2.7.x.
 *	Controls startup/shutdown of Python via the C Python API.
 *	Not a place for providing other interfaces or storing global script stuff.
 */
class Python27ScriptingEngine
	: public Implements< IPythonScriptingEngine > // Always implement latest version
{
public:
	Python27ScriptingEngine( IComponentContext & context );
	virtual ~Python27ScriptingEngine();


	/**
	 *	The Python interpreter must be initialized before it can be used.
	 *	@return true on success.
	 */
	bool init();


	/**
	 *	Free all resources used by Python before execution ends.
	 *	If the interpreter was not initialized, it should do nothing.
	 */
	void fini();

	bool appendSourcePath( const wchar_t * path ) override;
	bool appendBinPath( const wchar_t * path ) override;
	ObjectHandle import( const char * moduleName ) override;
	bool checkErrors() override;

private:
	Python27ScriptingEngine( const Python27ScriptingEngine & other );
	Python27ScriptingEngine( Python27ScriptingEngine && other );

	Python27ScriptingEngine & operator=( const Python27ScriptingEngine & other );
	Python27ScriptingEngine & operator=( Python27ScriptingEngine && other );

	IComponentContext & context_;
};
} // end namespace wgt
#endif // PYTHON_SCRIPTING_ENGINE_HPP
