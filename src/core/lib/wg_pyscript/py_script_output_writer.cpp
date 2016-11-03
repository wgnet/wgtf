#include "pch.hpp"

#include "py_script_output_writer.hpp"
#include "py_script_object.hpp"

#include "core_logging/logging.hpp"

#include <cassert>
#include <methodobject.h>
#include <object.h>


namespace wgt
{
namespace PyScript
{


void handleWrite( const std::string & msg, bool isStderr );


/**
 *	This method implements the Python write method. It is used to redirect the
 *	write calls to this object's pOwner_.
 */
static PyObject* write(PyObject* self, PyObject* args, bool isStderr)
{
	char * msg = NULL;
	Py_ssize_t msglen = 0;

	if (!PyArg_ParseTuple( args, "s#", &msg, &msglen ))
	{
		NGT_ERROR_MSG("Bad args\n");
		return NULL;
	}

	// Handle embedded NULL (i.e. not UTF-8)
	if (memchr( msg, '\0', msglen ))
	{
		ScriptObject repr = ScriptObject( PyObject_Repr(
				PyTuple_GET_ITEM( args, 0 ) ),
			ScriptObject::FROM_NEW_REFERENCE );

		handleWrite(std::string(PyString_AsString(repr.get()),
		                        PyString_GET_SIZE(repr.get())),
		            isStderr);

		Py_RETURN_NONE;
	}

	handleWrite(std::string(msg, msglen), isStderr);
	Py_RETURN_NONE;
}

static PyObject* writeStdout(PyObject* self, PyObject* args)
{
	const auto isStderr = false;
	return write(self, args, isStderr);
}

static PyObject* writeStderr(PyObject* self, PyObject* args)
{
	const auto isStderr = true;
	return write(self, args, isStderr);
}

static PyObject* flush(PyObject* self, PyObject* args)
{
	NGT_FLUSH_MSG();
	Py_RETURN_NONE;
}

// -----------------------------------------------------------------------------
// Section: PythonOutputWriter
// -----------------------------------------------------------------------------
// Note: we don't actually lock the GIL, we're assuming our locking is correct.
// See PythonOutputWriter::addHook and PythonOutputWriter::delHook for details


/**
 *	Overrides the stdout and stderr members of the
 *	sys module with PyOutputStreams that redirect to this instance.
 */
PyMODINIT_FUNC PyInit_ScriptOutputWriter()
{
	assert( Py_IsInitialized() );

	static PyMethodDef s_stdoutMethods[] =
	{
	  { "write", &writeStdout, METH_VARARGS, "Write text to console" },
	  { "flush", &flush, METH_VARARGS, "Flush text to console" },
	  { NULL, NULL, 0, NULL }
	};

	static PyMethodDef s_stderrMethods[] =
	{
	  { "write", &writeStderr, METH_VARARGS, "Write error to console" },
	  { "flush", &flush, METH_VARARGS, "Flush error to console" },
	  { NULL, NULL, 0, NULL }
	};

	PyScript::ScriptModule stdoutModule(Py_InitModule("stdoutwriter", s_stdoutMethods),
	                                    PyScript::ScriptObject::FROM_BORROWED_REFERENCE);
	assert(stdoutModule.exists());
	PyScript::ScriptModule stderrModule(Py_InitModule("stderrwriter", s_stderrMethods),
	                                    PyScript::ScriptObject::FROM_BORROWED_REFERENCE);
	assert(stderrModule.exists());
	auto combinedModule = PyScript::ScriptModule::getOrCreate("scriptoutputwriter",
	                                                          PyScript::ScriptErrorPrint());
	combinedModule.addObject("stdout", stdoutModule, PyScript::ScriptErrorPrint());
	combinedModule.addObject("stderr", stderrModule, PyScript::ScriptErrorPrint());

	if (stdoutModule != nullptr)
	{
		const int stdoutSet = PySys_SetObject("stdout", stdoutModule.get());
		assert( stdoutSet == 0 );
		const int stderrSet = PySys_SetObject("stderr", stderrModule.get());
		assert( stderrSet == 0 );
	}
}


/**
 *	This method assembles a buffer from the input, and once it has a terminating
 *	newline, flushes it to the logs.
 *
 *	This method is for the private use of PyOutputStream.
 *
 *	@param msg	The input given to the calling PyOutputStream
 *	@param isStderr	Whether the input was given on stderr or equivalent.
 */
void handleWrite( const std::string & msg, bool isStderr )
{
	static std::string			stdoutBuffer_;
	static std::string			stderrBuffer_;

	std::string & buffer = isStderr ? stderrBuffer_ : stdoutBuffer_;

	buffer += msg;

	if (!buffer.empty() && buffer[ buffer.size() - 1 ] == '\n')
	{
		// This is done so that the hack to prefix the time in cell and the base
		// applications works (needs a \n in the format string).
		buffer.resize( buffer.size() - 1 );

		std::string::size_type lineStart = 0;

		do
		{
			std::string::size_type lineEnd = buffer.find( '\n', lineStart );

			std::string line( buffer.data() + lineStart,
				(lineEnd != std::string::npos) ? 
					(lineEnd - lineStart) : (buffer.size() - lineStart) );

			if (isStderr)
			{
				NGT_ERROR_MSG( "%s\n", line.c_str() );
			}
			else
			{
				NGT_TRACE_MSG( "%s\n", line.c_str() );
			}

			lineStart = (lineEnd != std::string::npos) ? 
				(lineEnd + 1) : std::string::npos;
		}
		while (lineStart != std::string::npos);

		buffer = "";
	}
}


} // namespace PyScript


// py_script_output_writer.cpp
} // end namespace wgt
