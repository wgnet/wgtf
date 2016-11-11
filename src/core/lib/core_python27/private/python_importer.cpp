/*
 *
 *  Copyright (C) 2010 MeVis Medical Solutions AG All Rights Reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  Further, this software is distributed without any warranty that it is
 *  free of the rightful claim of any third person regarding infringement
 *  or the like.  Any license provided herein, whether implied or
 *  otherwise, applies only to this software file.  Patent licenses, if
 *  any, provided herein do not apply to combinations of this program with
 *  other software, or any other product whatsoever.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Contact information: MeVis Medical Solutions AG, Universitaetsallee 29,
 *  28359 Bremen, Germany or:
 *
 *  http://www.mevis.de
 *
 */
// https://sourceforge.net/p/pythonqt/code/HEAD/tree/trunk/src/PythonQtImporter.cpp

#include "pch.hpp"

#if ENABLE_DEPLOY_PYTHON_SUPPORT
#include "wg_pyscript/py_script_object.hpp"
#include "python_importer.hpp"
#include "wg_types/binary_block.hpp"

#include <algorithm>
#include <cstring>
#include <cwchar>
#include "marshal.h"
#include "osdefs.h"

namespace wgt
{
namespace PythonImport_Detail
{
// Begin: from Python/importdl.h
/* Definitions for dynamic loading of extension modules */
enum filetype
{
	SEARCH_ERROR,
	PY_SOURCE,
	PY_COMPILED,
	C_EXTENSION,
	PY_RESOURCE, /* Mac only */
	PKG_DIRECTORY,
	C_BUILTIN,
	PY_FROZEN,
	PY_CODERESOURCE, /* Mac only */
	IMP_HOOK
};

// End: from Python/importdl.h
/**
 * Check if the given file type can be imported by WGTPythonImporter.
 */
bool isFileTypeSupported(filetype type)
{
	return (type == PY_SOURCE) || (type == PY_COMPILED) || (type == PKG_DIRECTORY);
}

struct wgt_module_searchorder
{
	char suffix[14];
	filetype type;
};

/**
 *	@note IMPORTANT check that any changes to this struct match the hard-coded
 *	offsets in WGTPythonImport::init().
 */
struct wgt_module_searchorder module_searchorder[] = {
	{ "/__init__.pyc", PKG_DIRECTORY },
	{ "/__init__.pyo", PKG_DIRECTORY }, // See Py_OptimizeFlag
	{ "/__init__.py", PKG_DIRECTORY },
	{ ".pyc", PY_COMPILED },
	{ ".pyo", PY_COMPILED }, // See Py_OptimizeFlag
	{ ".py", PY_SOURCE },
#ifdef MS_WINDOWS
#ifdef _DEBUG
	{ "_d.pyd", C_EXTENSION },
#else
	{ ".pyd", C_EXTENSION },
#endif
#endif // MS_WINDOWS
	{ "", SEARCH_ERROR } // Sentinel
};

struct ModuleInfo
{
	ModuleInfo()
	{
		type_ = SEARCH_ERROR;
	}
	std::string fullPath; //!< the full path to the found file
	std::string moduleName; //!< the module name without the package prefix
	enum filetype type_;
};

PyScript::ScriptCode unmarshalCode(const char* data, int size);
PyScript::ScriptCode getCodeFromData(const std::string& path, const char* data, int size, filetype type);
PyScript::ScriptCode getModuleCode(const ModuleInfo& info);
ModuleInfo getModuleInfo(WGTPythonImporter* self, const char* fullname);
std::string getSubName(const std::string& str);
long getLong(unsigned char* buf);
std::string replaceExtension(const std::string& str, const char* ext);

extern PyTypeObject WGTPythonImporter_Type;
PyObject* WGTPythonImportError;

int WGTPythonImporter_init(WGTPythonImporter* self, PyObject* args, PyObject* /*kwds*/)
{
	self->_path = NULL;

	const char* cpath;
	if (!PyArg_ParseTuple(args, "s", &cpath))
		return -1;
	self->_path = (char*)malloc(strlen(cpath) + 1);
	strcpy(self->_path, cpath);
	return 0;
}

void WGTPythonImporter_dealloc(WGTPythonImporter* self)
{
	// free the stored path
	if (self->_path)
	{
		free(self->_path);
		self->_path = NULL;
	}
	// free ourself
	Py_TYPE(self)->tp_free((PyObject*)self);
}

PyObject* WGTPythonImporter_find_module(PyObject* obj, PyObject* args)
{
	WGTPythonImporter* self = (WGTPythonImporter*)obj;
	PyObject* path = NULL;
	char* fullname;

	if (!PyArg_ParseTuple(args, "s|O:WGTPythonImporter.find_module", &fullname, &path))
		return NULL;

	ModuleInfo info = PythonImport_Detail::getModuleInfo(self, fullname);
	if (info.type_ != SEARCH_ERROR)
	{
		Py_INCREF(self);
		return (PyObject*)self;
	}
	else
	{
		Py_INCREF(Py_None);
		return Py_None;
	}
}

PyObject* WGTPythonImporter_iter_modules(PyObject* obj, PyObject* args)
{
	// implement this if needed
	return NULL;
}

bool setModuleProperties(WGTPythonImporter* self, const ModuleInfo& info, PyScript::ScriptModule& mod)
{
	auto dict = mod.getDict();

	PyScript::ScriptObject selfPyObject(reinterpret_cast<PyObject*>(self),
	                                    PyScript::ScriptObject::FROM_BORROWED_REFERENCE);
	if (!dict.setItem("__loader__", selfPyObject, PyScript::ScriptErrorRetain()))
	{
		return false;
	}

	if (info.type_ == PKG_DIRECTORY)
	{
		PyScript::ScriptString fullpath(PyString_FromFormat("%s%c%s", self->_path, SEP, info.moduleName.c_str()),
		                                PyScript::ScriptObject::FROM_NEW_REFERENCE);
		if (!fullpath.exists())
		{
			return false;
		}

		auto pkgpath = PyScript::ScriptList::create();
		if (!pkgpath.exists())
		{
			return false;
		}
		if (!pkgpath.append(fullpath))
		{
			return false;
		}
		if (!dict.setItem("__path__", pkgpath, PyScript::ScriptErrorRetain()))
		{
			return false;
		}
	}

	return true;
}

PyObject* WGTPythonImporter_load_module(PyObject* obj, PyObject* args)
{
	WGTPythonImporter* self = (WGTPythonImporter*)obj;
	char* fullname;

	if (!PyArg_ParseTuple(args, "s:WGTPythonImporter.load_module", &fullname))
	{
		return nullptr;
	}

	const auto info = PythonImport_Detail::getModuleInfo(self, fullname);
	if (info.type_ == SEARCH_ERROR)
	{
		return nullptr;
	}

	// Use Qt resource for supported module types
	if (isFileTypeSupported(info.type_))
	{
		auto code = PythonImport_Detail::getModuleCode(info);
		if (!code.exists())
		{
			return nullptr;
		}

		auto mod = PyScript::ScriptModule::getOrCreate(fullname, PyScript::ScriptErrorRetain());
		if (!mod.exists())
		{
			return nullptr;
		}
		if (!PythonImport_Detail::setModuleProperties(self, info, mod))
		{
			return nullptr;
		}
		mod = PyScript::ScriptModule(PyImport_ExecCodeModuleEx(fullname, code.get(), (char*)info.fullPath.c_str()),
		                             PyScript::ScriptObject::FROM_NEW_REFERENCE);
		return mod.newRef();
	}

	// Use default importer for unsupported module types
	PyScript::ScriptList origPathHooks(PySys_GetObject(const_cast<char*>("path_hooks")),
	                                   PyScript::ScriptObject::FROM_BORROWED_REFERENCE);

	PyScript::ScriptDict origImporterCache(PySys_GetObject(const_cast<char*>("path_importer_cache")),
	                                       PyScript::ScriptObject::FROM_BORROWED_REFERENCE);

	{
		auto emptyPathHooks = PyScript::ScriptList::create();
		const int hooksResult = PySys_SetObject("path_hooks", emptyPathHooks.get());
		if (hooksResult != 0)
		{
			return nullptr;
		}

		auto emptyCache = PyScript::ScriptDict::create();
		const int cacheResult = PySys_SetObject("path_importer_cache", emptyCache.get());
		if (cacheResult != 0)
		{
			return nullptr;
		}
	}

	auto mod = PyScript::ScriptModule::import(fullname, PyScript::ScriptErrorPrint());

	// Restore path hooks
	{
		const int cacheResult = PySys_SetObject("path_importer_cache", origImporterCache.get());
		if (cacheResult != 0)
		{
			return nullptr;
		}
		const int hooksResult = PySys_SetObject("path_hooks", origPathHooks.get());
		if (hooksResult != 0)
		{
			return nullptr;
		}
	}

	if (!PythonImport_Detail::setModuleProperties(self, info, mod))
	{
		return nullptr;
	}
	return mod.newRef();
}

PyObject* WGTPythonImporter_get_data(PyObject* /*obj*/, PyObject* /*args*/)
{
	// implement this if needed
	return NULL;
}

PyObject* WGTPythonImporter_get_code(PyObject* obj, PyObject* args)
{
	WGTPythonImporter* self = (WGTPythonImporter*)obj;
	char* fullname;

	if (!PyArg_ParseTuple(args, "s:WGTPythonImporter.get_code", &fullname))
		return NULL;

	const auto info = PythonImport_Detail::getModuleInfo(self, fullname);
	if (info.type_ == SEARCH_ERROR)
	{
		return nullptr;
	}
	return PythonImport_Detail::getModuleCode(info).newRef();
}

PyObject* WGTPythonImporter_get_source(PyObject* /*obj*/, PyObject* /*args*/)
{
	// implement this if needed
	return NULL;
}

PyMethodDef WGTPythonImporter_methods[] = { { "find_module", WGTPythonImporter_find_module, METH_VARARGS, NULL },
	                                        { "load_module", WGTPythonImporter_load_module, METH_VARARGS, NULL },
	                                        { "iter_modules", WGTPythonImporter_iter_modules, METH_VARARGS, NULL },
	                                        { "get_data", WGTPythonImporter_get_data, METH_VARARGS, NULL },
	                                        { "get_code", WGTPythonImporter_get_code, METH_VARARGS, NULL },
	                                        { "get_source", WGTPythonImporter_get_source, METH_VARARGS, NULL },
	                                        { NULL, NULL, 0, NULL } };

#define DEFERRED_ADDRESS(ADDR) 0

PyTypeObject WGTPythonImporter_Type = {
	PyVarObject_HEAD_INIT(DEFERRED_ADDRESS(&PyType_Type), 0) "WGTPythonImport.WGTPythonImporter",
	sizeof(WGTPythonImporter),
	0, /* tp_itemsize */
	(destructor)WGTPythonImporter_dealloc, /* tp_dealloc */
	0, /* tp_print */
	0, /* tp_getattr */
	0, /* tp_setattr */
	0, /* tp_compare */
	0, /* tp_repr */
	0, /* tp_as_number */
	0, /* tp_as_sequence */
	0, /* tp_as_mapping */
	0, /* tp_hash */
	0, /* tp_call */
	0, /* tp_str */
	PyObject_GenericGetAttr, /* tp_getattro */
	0, /* tp_setattro */
	0, /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
	0, /* tp_doc */
	0, /* tp_traverse */
	0, /* tp_clear */
	0, /* tp_richcompare */
	0, /* tp_weaklistoffset */
	0, /* tp_iter */
	0, /* tp_iternext */
	WGTPythonImporter_methods, /* tp_methods */
	0, /* tp_members */
	0, /* tp_getset */
	0, /* tp_base */
	0, /* tp_dict */
	0, /* tp_descr_get */
	0, /* tp_descr_set */
	0, /* tp_dictoffset */
	(initproc)WGTPythonImporter_init, /* tp_init */
	PyType_GenericAlloc, /* tp_alloc */
	PyType_GenericNew, /* tp_new */
	PyObject_Del, /* tp_free */
};

} // end namespace PythonImport_Detail

void WGTPythonImport::init()
{
	static bool first = true;
	if (!first)
	{
		return;
	}
	first = false;

	PyObject* mod;
	// def WGTPythonImporter(object): pass
	if (PyType_Ready(&PythonImport_Detail::WGTPythonImporter_Type) < 0)
		return;

	/* Correct directory separator */
	PythonImport_Detail::module_searchorder[0].suffix[0] = SEP;
	PythonImport_Detail::module_searchorder[1].suffix[0] = SEP;
	PythonImport_Detail::module_searchorder[2].suffix[0] = SEP;
	if (Py_OptimizeFlag)
	{
		/* Reverse *.pyc and *.pyo */
		struct PythonImport_Detail::wgt_module_searchorder tmp;
		tmp = PythonImport_Detail::module_searchorder[0];
		PythonImport_Detail::module_searchorder[0] = PythonImport_Detail::module_searchorder[1];
		PythonImport_Detail::module_searchorder[1] = tmp;
		tmp = PythonImport_Detail::module_searchorder[3];
		PythonImport_Detail::module_searchorder[3] = PythonImport_Detail::module_searchorder[4];
		PythonImport_Detail::module_searchorder[4] = tmp;
	}
	// WGTPythonImporter.py
	mod = Py_InitModule4("WGTPythonImport", NULL, NULL, NULL, PYTHON_API_VERSION);

	PythonImport_Detail::WGTPythonImportError =
	PyErr_NewException(const_cast<char*>("WGTPythonImport.WGTPythonImportError"), PyExc_ImportError, NULL);
	if (PythonImport_Detail::WGTPythonImportError == NULL)
		return;
	// import WGTPythonImportError
	Py_INCREF(PythonImport_Detail::WGTPythonImportError);
	if (PyModule_AddObject(mod, "WGTPythonImportError", PythonImport_Detail::WGTPythonImportError) < 0)
		return;
	// import WGTPythonImporter
	Py_INCREF(&PythonImport_Detail::WGTPythonImporter_Type);
	if (PyModule_AddObject(mod, "WGTPythonImporter", (PyObject*)&PythonImport_Detail::WGTPythonImporter_Type) < 0)
		return;

	// set our importer into the path_hooks to handle all path on sys.path
	// classobj = WGTPythonImporter
	PyObject* classobj = PyDict_GetItemString(PyModule_GetDict(mod), "WGTPythonImporter");

	// sys.path_hooks.append(classobj)
	PyObject* path_hooks = PySys_GetObject(const_cast<char*>("path_hooks"));
	PyList_Append(path_hooks, classobj);
}

bool WGTPythonImport::appendPath(const wchar_t* path)
{
	if ((path == nullptr) || (wcslen(path) == 0))
	{
		// Already added
		return true;
	}

// Normalize path
// Replace '/' with '\\' on Windows
// Because Python always uses the platform's native format
#if defined(WIN32)
	std::wstring tmpPath(path);
	std::replace(std::begin(tmpPath), std::end(tmpPath), L'/', L'\\');
	const auto normalizedPath = tmpPath.c_str();
#else // defined( WIN32 )
	const auto normalizedPath = path;
#endif // defined( WIN32 )

	PyScript::ScriptObject testPathObject = PyScript::ScriptObject::createFrom(normalizedPath);

	PyObject* pySysPaths = PySys_GetObject("path");
	if (pySysPaths == nullptr)
	{
		NGT_ERROR_MSG("Unable to get sys.path\n");
		return false;
	}

	PyScript::ScriptList sysPaths(pySysPaths);

	// Check for duplicates
	for (PyScript::ScriptList::size_type i = 0; i < sysPaths.size(); ++i)
	{
		auto item = sysPaths.getItem(i);
		if (item.compareTo(testPathObject, PyScript::ScriptErrorPrint()) == 0)
		{
			// Already added
			return true;
		}
	}

	// Add new path
	sysPaths.append(testPathObject);

	const int result = PySys_SetObject("path", pySysPaths);
	if (result != 0)
	{
		NGT_ERROR_MSG("Unable to assign sys.path\n");
		return false;
	}

	return true;
}

namespace PythonImport_Detail
{
PyScript::ScriptCode unmarshalCode(const char* data, int size)
{
	// ugly cast, but Python API is not const safe
	char* buf = (char*)data;

	if (size <= 9)
	{
		if (Py_VerboseFlag)
		{
			PySys_WriteStderr("# it has bad pyc data\n");
		}
		return PyScript::ScriptCode(nullptr);
	}

	if (getLong((unsigned char*)buf) != PyImport_GetMagicNumber())
	{
		if (Py_VerboseFlag)
		{
			PySys_WriteStderr("# it has bad magic\n");
		}
		return PyScript::ScriptCode(nullptr);
	}

	PyScript::ScriptCode code(PyMarshal_ReadObjectFromString(buf + 8, size - 8),
	                          PyScript::ScriptObject::FROM_NEW_REFERENCE);
	return code;
}

PyScript::ScriptCode getCodeFromData(const std::string& path, const char* data, int size, filetype type)
{
	if (Py_VerboseFlag > 1)
	{
		PySys_WriteStderr("# trying %s\n", path.c_str());
	}

	filetype compileType = type;
	if (type == PKG_DIRECTORY)
	{
		// Check if "pkg/__init__.pyc" or "pkg/__init__.py"
		if (path.rfind("pyc") != std::string::npos)
		{
			compileType = PY_COMPILED;
		}
		else
		{
			compileType = PY_SOURCE;
		}
	}

	if (compileType == PY_SOURCE)
	{
		std::string nullTerminatedData(data, size);
		return PyScript::ScriptCode(Py_CompileString(nullTerminatedData.c_str(), path.c_str(), Py_file_input),
		                            PyScript::ScriptObject::FROM_NEW_REFERENCE);
	}
	else if (compileType == PY_COMPILED)
	{
		return PythonImport_Detail::unmarshalCode(data, size);
	}
	return PyScript::ScriptCode(nullptr);
}

PyScript::ScriptCode getModuleCode(const ModuleInfo& info)
{
	assert(info.type_ != SEARCH_ERROR);

	auto pResourceSystem = getPythonDependency<IResourceSystem>();
	assert(pResourceSystem != nullptr);

	std::unique_ptr<BinaryBlock> blocks = pResourceSystem->readBinaryContent(info.fullPath.c_str());
	return getCodeFromData(info.fullPath, blocks->cdata(), static_cast<int>(blocks->length()), info.type_);
}

ModuleInfo getModuleInfo(WGTPythonImporter* self, const char* fullname)
{
	auto resourceSystem = getPythonDependency<IResourceSystem>();
	assert(resourceSystem != nullptr);
	ModuleInfo info;
	std::string subname;
	struct wgt_module_searchorder* zso;

	subname = getSubName(fullname);
	std::string path = self->_path;
	path.append(1, SEP);
	path.append(subname);
#if defined(WIN32)
	std::replace(std::begin(path), std::end(path), '/', '\\');
#endif // defined( WIN32 )
	std::string test;
	for (zso = module_searchorder; *zso->suffix; zso++)
	{
		test = path + zso->suffix;
		if (resourceSystem->exists(test.c_str()))
		{
			info.fullPath = test;
			info.moduleName = subname;
			info.type_ = zso->type;
			return info;
		}
	}
	return info;
}

std::string getSubName(const std::string& str)
{
	int idx = static_cast<int>(str.find_last_of('.'));
	if (idx != -1)
	{
		return str.substr(idx + 1);
	}
	else
	{
		return str;
	}
}

/* Given a buffer, return the long that is represented by the first
   4 bytes, encoded as little endian. This partially reimplements
   marshal.c:r_long() */
long getLong(unsigned char* buf)
{
	long x;
	x = buf[0];
	x |= (long)buf[1] << 8;
	x |= (long)buf[2] << 16;
	x |= (long)buf[3] << 24;
#if SIZEOF_LONG > 4
	/* Sign extension for 64-bit machines */
	x |= -(x & 0x80000000L);
#endif
	return x;
}

std::string replaceExtension(const std::string& str, const char* ext)
{
	std::string r;
	int i = static_cast<int>(str.find_last_of('.'));
	if (i != -1)
	{
		r = str.substr(0, i) + "." + ext;
	}
	else
	{
		r = str + "." + ext;
	}
	return r;
}
} // end namespace PythonImport_Detail
} // end namespace wgt
#endif // ENABLE_DEPLOY_PYTHON_SUPPORT
