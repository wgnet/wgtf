#include "pch.hpp"
#include "type_converter.hpp"
#include "pyobject_pointer.hpp"

#include "core_logging/logging.hpp"

#ifndef _WIN32
	#define _stricmp strcasecmp
#endif

namespace wgt
{
namespace PyScript
{


/// @see //lib/pyscript/script.cpp
namespace Script
{


/**
 *	This function tries to interpret its argument as a boolean,
 *	setting it if it is, and generating an exception otherwise.
 *
 *	@return 0 for success, -1 for error (like pySetAttribute)
 */
int setData( PyObject * pObject, bool & rBool,
	const char * varName )
{
	if (PyInt_Check( pObject ))
	{
		rBool = PyInt_AsLong( pObject ) != 0;
		return 0;
	}

	if (PyString_Check( pObject ))
	{
		char * pStr = PyString_AsString( pObject );
		if (!_stricmp( pStr, "true" ))
		{
			rBool = true;
			return 0;
		}
		else if(!_stricmp( pStr, "false" ))
		{
			rBool = false;
			return 0;
		}
	}

	PyErr_Format( PyExc_TypeError, "%s must be set to a bool", varName );
	return -1;
}


/**
 *	This function tries to interpret its argument as an integer,
 *	setting it if it is, and generating an exception otherwise.
 *
 *	@return 0 for success, -1 for error (like pySetAttribute)
 */
int setData( PyObject * pObject, int & rInt,
	const char * varName )
{
	if (PyInt_Check( pObject ))
	{
		long asLong = PyInt_AsLong( pObject );
		rInt = asLong;

		if (asLong == rInt)
		{
			return 0;
		}
	}

	if (PyFloat_Check( pObject ))
	{
		rInt = (int)PyFloat_AsDouble( pObject );
		return 0;
	}

	if (PyLong_Check( pObject ))
	{
		long asLong = PyLong_AsLong( pObject );
		rInt = int( asLong );

		if (!PyErr_Occurred())
		{
			if (rInt == asLong)
			{
				return 0;
			}
		}
		else
		{
			PyErr_Clear();
		}
	}

	PyErr_Format( PyExc_TypeError, "%s must be set to an int", varName );
	return -1;

}


/**
 *	This function tries to interpret its argument as an integer,
 *	setting it if it is, and generating an exception otherwise.
 *
 *	@return 0 for success, -1 for error (like pySetAttribute)
 */
int setData( PyObject * pObject, long & rInt,
					const char * varName )
{
	typedef int64_t storage_type;
	static_assert( sizeof( storage_type ) >= sizeof( long ), "Loss of data" );

	storage_type value;
	int result = setData( pObject, value, varName );
	if (result == 0)
	{
		rInt = static_cast< long >( value );
	}
	return result;
}


/**
 *	This function tries to interpret its argument as an integer,
 *	setting it if it is, and generating an exception otherwise.
 *
 *	@return 0 for success, -1 for error (like pySetAttribute)
 */
int setData( PyObject * pObject, int64_t & rInt,
	const char * varName )
{
	if (PyLong_Check( pObject ))
	{
		rInt = PyLong_AsLongLong( pObject );
		if (!PyErr_Occurred()) return 0;
	}

	if (PyInt_Check( pObject ))
	{
		rInt = PyInt_AsLong( pObject );
		return 0;
	}

	if (PyFloat_Check( pObject ))
	{
		rInt = (int64_t)PyFloat_AsDouble( pObject );
		return 0;
	}

	PyErr_Format( PyExc_TypeError, "%s must be set to a long", varName );
	return -1;
}

/**
 *	This function tries to interpret its argument as an unsigned
 *	64-bit integer, setting it if it is, and generating an exception
 *	otherwise.
 *
 *	@return 0 for success, -1 for error (like pySetAttribute)
 */
int setData( PyObject * pObject, uint64_t & rUint,
	const char * varName )
{
	if (PyLong_Check( pObject ))
	{
		rUint = PyLong_AsUnsignedLongLong( pObject );
		if (!PyErr_Occurred()) return 0;
	}

	if (PyInt_Check( pObject ))
	{
		long intValue = PyInt_AsLong( pObject );
		if (intValue >= 0)
		{
			rUint = (uint64_t)intValue;
			return 0;
		}
		else
		{
			PyErr_Format( PyExc_ValueError,
				"Cannot set %s of type unsigned long to %d",
				varName, int(intValue) );
			return -1;
		}
	}

	if (PyFloat_Check( pObject ))
	{
		rUint = (uint64_t)PyFloat_AsDouble( pObject );
		return 0;
	}

	PyErr_Format( PyExc_TypeError,
			"%s must be set to a unsigned long", varName );
	return -1;
}


/**
 *	This function tries to interpret its argument as an unsigned integer,
 *	setting it if it is, and generating an exception otherwise.
 *
 *	@return 0 for success, -1 for error (like pySetAttribute)
 */
int setData( PyObject * pObject, unsigned int & rUint,
	const char * varName )
{
	if (PyInt_Check( pObject ))
	{
		long longValue = PyInt_AsLong( pObject );
		rUint = longValue;

		if ((longValue >= 0) && (static_cast< long >( rUint ) == longValue))
		{
			return 0;
		}
	}

	if (PyFloat_Check( pObject ))
	{
		rUint = (int)PyFloat_AsDouble( pObject );
		return 0;
	}

	if (PyLong_Check( pObject ))
	{
		unsigned long asUnsignedLong = PyLong_AsUnsignedLong( pObject );
		rUint = unsigned int( asUnsignedLong );
		if (!PyErr_Occurred() &&
				(rUint == asUnsignedLong))
		{
			return 0;
		}
		PyErr_Clear();

		long asLong = PyLong_AsLong( pObject );
		rUint = unsigned int( asLong );
		if (!PyErr_Occurred() &&
				(asLong >= 0) &&
				(asLong == static_cast< long >( rUint ) ))
		{
			return 0;
		}
	}

	PyErr_Format( PyExc_TypeError, "%s must be set to an uint", varName );
	return -1;

}


/**
 *	This function tries to interpret its argument as a float,
 *	setting it if it is, and generating an exception otherwise.
 *
 *	@return 0 for success, -1 for error (like pySetAttribute)
 */
int setData( PyObject * pObject, float & rFloat,
	const char * varName )
{
	double	d;
	int ret = setData( pObject, d, varName );
	if (ret == 0) rFloat = float(d);
	return ret;
}


/**
 *	This function tries to interpret its argument as a double,
 *	setting it if it is, and generating an exception otherwise.
 *
 *	@return 0 for success, -1 for error (like pySetAttribute)
 */
int setData( PyObject * pObject, double & rDouble,
	const char * varName )
{
	if (PyFloat_Check( pObject ))
	{
		rDouble = PyFloat_AsDouble( pObject );
		return 0;
	}

	if (PyInt_Check( pObject ))
	{
		rDouble = PyInt_AsLong( pObject );
		return 0;
	}

	if (PyLong_Check( pObject ))
	{
		rDouble = PyLong_AsUnsignedLong( pObject );
		if (!PyErr_Occurred()) return 0;
	}

	PyErr_Format( PyExc_TypeError, "%s must be set to a float", varName );
	return -1;

}


/**
 *	This function tries to interpret its argument as a PyObject,
 *	setting it if it is, and generating an exception otherwise.
 *
 *	It handles reference counting properly, but it does not check
 *	the type of the input object.
 *
 *	None is translated into NULL.
 *
 *	@return 0 for success, -1 for error (like pySetAttribute)
 */
int setData( PyObject * pObject, PyObject * & rPyObject,
	const char * /*varName*/ )
{
	PyObject * inputObject = rPyObject;

	rPyObject = (pObject != Py_None) ? pObject : NULL;

	Py_XINCREF( rPyObject );

	if (inputObject)
	{
		NGT_WARNING_MSG( "setData( pObject , rPyObject ): "
			"rPyObject is not NULL and is DECREFed and replaced by pObject\n" );
	}
	Py_XDECREF( inputObject );

	return 0;
}


/**
 *	This function tries to interpret its argument as a SmartPointer<PyObject>,
 *	setting it if it is, and generating an exception otherwise.
 *
 *	@see setData of PyObject * &
 */
int setData( PyObject * pObject, PyObjectPtr & rPyObject,
	const char * /*varName*/ )
{
	PyObject * pSet = (pObject != Py_None) ? pObject : NULL;

	if (rPyObject.get() != pSet) rPyObject = pSet;

	return 0;
}


/**
 *	This function tries to interpret its argument as a string
 *	setting it if it is, and generating an exception otherwise.
 *
 *	@return 0 for success, -1 for error (like pySetAttribute)
 */
int setData( PyObject * pObject, std::string & rString,
	const char * varName )
{
	PyObjectPtr pUTF8String;

	if (PyUnicode_Check( pObject ))
	{
		pUTF8String = PyObjectPtr( PyUnicode_AsUTF8String( pObject ),
			true /*STEAL_REFERENCE*/ );
		pObject = pUTF8String.get();
		if (pObject == NULL)
		{
			return -1;
		}
	}

	if (!PyString_Check( pObject ))
	{
		PyErr_Format( PyExc_TypeError, "%s must be set to a string.", varName );
		return -1;
	}

	char *ptr_cs;
	Py_ssize_t len_cs;
	PyString_AsStringAndSize( pObject, &ptr_cs, &len_cs );
	rString.assign( ptr_cs, len_cs );
	return 0;
}


/**
 *	This function tries to interpret its argument as a wide string
 *	setting it if it is, and generating an exception otherwise.
 *
 *	@return 0 for success, -1 for error (like pySetAttribute)
 */
int setData( PyObject * pObject, std::wstring & rString,
	const char * varName )
{
	if (PyString_Check( pObject ) || PyUnicode_Check( pObject ))
	{
		PyObjectPtr pUO( PyObject_Unicode( pObject ), true );

		if (pUO)
		{
			PyUnicodeObject* unicodeObj = reinterpret_cast<PyUnicodeObject*>(pUO.get());

			Py_ssize_t ulen = PyUnicode_GET_DATA_SIZE( unicodeObj ) / sizeof(Py_UNICODE);
			if (ulen >= 0)
			{
				// In theory this is bad, because we're assuming that 
				// sizeof(Py_UNICODE) == sizeof(wchar_t), and that ulen maps to
				// of characters that PyUnicode_AsWideChar will write into the 
				// destination buffer. In practice this is true, but for good measure
				// I'm going to stick in a compile-time assert.
				static_assert( sizeof(Py_UNICODE) == sizeof(wchar_t), "SizeOfPyUnicodeIsNotSizeOfWchar_t" );
				rString.resize(ulen);

				if (rString.empty())
				{
					return 0;
				}

				Py_ssize_t nChars = 
					PyUnicode_AsWideChar( unicodeObj, &rString[0], ulen );

				if ( nChars != -1 )
				{
					return 0;
				}
			}
		}
	}

	PyErr_Format( PyExc_TypeError,
			"%s must be set to a wide string.", varName );
	return -1;

}


/**
 * This function makes a PyObject from a bool
 */
PyObject * getData( const bool data )
{
	return PyBool_FromLong( data );
}


/**
 * This function makes a PyObject from an int
 */
PyObject * getData( const int data )
{
	return PyInt_FromLong( data );
}


/**
 * This function makes a PyObject from an int
 */
PyObject * getData( const long data )
{
	return PyInt_FromLong( data );
}


/**
 * This function makes a PyObject from an unsigned int
 */
PyObject * getData( const unsigned int data )
{
	unsigned long asULong = data;

	return (long(asULong) < 0) ?
		PyLong_FromUnsignedLong( asULong ) :
		PyInt_FromLong( asULong );
}


/**
 * This function makes a PyObject form an int64
 */
PyObject * getData( const int64_t data )
{
	if (sizeof( int64_t ) == sizeof( long ))
	{
		return PyInt_FromLong( (long)data );
	}
	else
	{
		return PyLong_FromLongLong( data );
	}
}


/**
 * This function makes a PyObject form an uint64
 */
PyObject * getData( const uint64_t data )
{
	if (sizeof( int64_t ) == sizeof( long ))
	{
		unsigned long asULong = (unsigned long)data;

		if (long( asULong ) >= 0)
		{
			return PyInt_FromLong( asULong );
		}
	}

	return PyLong_FromUnsignedLongLong( data );
}


/**
 * This function makes a PyObject from a float
 */
PyObject * getData( const float data )
{
	return PyFloat_FromDouble( data );
}


/**
 * This function makes a PyObject from a double
 */
PyObject * getData( const double data )
{
	return PyFloat_FromDouble( data );
}


/**
 * This function makes a PyObject from a PyObject,
 *	and it handles reference counting properly.
 *
 *	NULL is translated into None
 */
PyObject * getData( const PyObject * data )
{
	PyObject * ret = (data != NULL) ? const_cast<PyObject*>( data ) : Py_None;
	Py_INCREF( ret );
	return ret;
}


/**
 * This function makes a PyObject from a ConstSmartPointer<PyObject>,
 *	and it handles reference counting properly.
 *
 *	@see getData for const PyObject *
 */
PyObject * getData( const PyObjectPtr data )
{
	PyObject * ret = (data ?
		const_cast<PyObject*>( data.get() ) : Py_None);
	Py_INCREF( ret );
	return ret;
}


/**
 *	This function makes a PyObject from a string.
 */
PyObject * getData( const std::string & data )
{
	PyObject * pRet = PyString_FromStringAndSize(
		const_cast<char *>( data.data() ), data.size() );

	return pRet;
}


/**
 *	This function makes a PyObject from a wide string.
 */
PyObject * getData( const std::wstring & data )
{
	PyObject * pRet = PyUnicode_FromWideChar(
		const_cast<wchar_t *>( data.c_str() ), data.size() );

	return pRet;
}


/**
 *	This function makes a PyObject from a const char *.
 */
PyObject * getData( const char * data )
{
	PyObject * pRet = PyString_FromString( const_cast<char *>( data ) );

	return pRet;
}


/**
 *	@pre data must be null-terminated.
 */
PyObject * getData( const wchar_t * data )
{
	PyObject * pRet = PyUnicode_FromWideChar( data, wcslen( data ) );

	return pRet;
}


} // namespace Script


} // namespace PyScript
} // end namespace wgt
