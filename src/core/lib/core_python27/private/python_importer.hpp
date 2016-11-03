#pragma once
#ifndef PYTHON_IMPORTER_HPP
#define PYTHON_IMPORTER_HPP

/**
 *	Only enable custom path_hooks in a deployment build.
 *	In development builds pydev requires non-mangled source paths to place
 *	break points.
 */
#if ENABLE_DEPLOY_PYTHON_SUPPORT

// Python.h must be first
#include "Python.h"
#include <string>
#include "core_dependency_system/depends.hpp"
#include "core_serialization/i_resource_system.hpp"

namespace wgt
{
	struct PythonDependencies
		: public Depends< IResourceSystem >
	{
		PythonDependencies( IComponentContext & context )
			: Depends( context )
		{
		}
	};
	extern std::unique_ptr< PythonDependencies > g_Deps;

	template<typename T> 
	T* getPythonDependency()
	{
		assert( g_Deps != nullptr );
		return g_Deps->get<T>();
	}

	typedef struct _WGTPythonImporter {
		PyObject_HEAD
			char* _path;
	} WGTPythonImporter;

	/**
	* A python module that implemented importing a python script from a resource package which used to replace
	* python internal import functionality
	*/
    namespace WGTPythonImport
    {
    void init();
    bool appendPath(const wchar_t* path);
    };
} // end namespace wgt
#endif // ENABLE_DEPLOY_PYTHON_SUPPORT

#endif // PYTHON_IMPORTER_HPP
