#ifndef PYTHON_OUTPUT_WRITER_HPP
#define PYTHON_OUTPUT_WRITER_HPP

#include "Python.h"

#include <string>

namespace wgt
{
namespace PyScript
{


/**
 *	This class is used to manage hooking of Python's stdio output.
 *
 *	Instances should only be created when initialising the Python interpreter.
 *	A new instance will automatically redirect sys.stdout and sys.stderr to
 *	itself.
 *
 *	Note: ReferenceCount is only for internal use, no code except
 *	PyOutputStream should be taking references to this class.
 */
//class ScriptOutputWriter
//{
//public:
//	ScriptOutputWriter();
//};

PyMODINIT_FUNC PyInit_ScriptOutputWriter();


} // namespace PyScript
} // end namespace wgt
#endif // PYTHON_OUTPUT_WRITER_HPP
