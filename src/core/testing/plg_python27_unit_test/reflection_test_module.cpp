#include "pch.hpp"

#include <longintrepr.h>

#include "reflection_test_module.hpp"

#include "core_python27/definition_details.hpp"
#include "core_python27/defined_instance.hpp"
#include "core_python27/type_converters/converters.hpp"

#include "core/interfaces/core_script/type_converter_queue.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/class_definition.hpp"
#include "core_reflection/object_handle_storage_shared.hpp"
#include "core_reflection/reflected_method_parameters.hpp"
#include "core_reflection/type_class_definition.hpp"


namespace wgt
{
namespace
{

//PlatformHelpers until we upgrade to VS 2015
namespace PlatformHelpers
{
	void sprintfSize_t(char * buffer, size_t value)
	{
#if _MSC_VER >= 1900
		sprintf(buffer, "%zd", value);
#else
		sprintf(buffer, "%d", value);
#endif /*_MSC_VER >= 1900 */
	}
}

/// State storage for static functions attached to Python
static ReflectionTestModule * g_module = nullptr;


/**
 *	Create a reflected object and return it to Python.
 *	
 *	@param self the reflection module.
 *	@param args first argument must be a TypeId.
 *		e.g. reflection.create("TypeId")
 *	@param argument keywords
 *		e.g. reflection.create(objectType="TypeId")
 *	
 *	@throw TypeError when arguments cannot be parsed.
 *	@throw TypeError when the TypeId does not have a valid definition.
 *	
 *	@return reflected object.
 */
static PyObject * py_create( PyObject * self, PyObject * args, PyObject * kw )
{
	if (g_module == nullptr)
	{
		PyErr_Format( PyExc_Exception,
			"Module is not loaded." );
		return nullptr;
	}
	const char * m_name = g_module->testName_;
	TestResult & result_ = g_module->result_;

	char * objectType = nullptr;

	static char *keywords [] = {
		"objectType",
		nullptr
	};

	if (!PyArg_ParseTupleAndKeywords( args, kw, "s", keywords, &objectType ))
	{
		// PyArg_ParseTupleAndKeywords sets error indicator
		return nullptr;
	}
	if (objectType[0] == '\0')
	{
		PyErr_Format( PyExc_TypeError,
			"Can not create a definition from an empty string." );
		return nullptr;
	}

	auto pDefinitionManager =
		g_module->context_.queryInterface< IDefinitionManager >();
	if (pDefinitionManager == nullptr)
	{
		PyErr_Format( PyExc_Exception,
			"Could not get definition manager." );
		return nullptr;
	}

	const IClassDefinition * pDefinition =
		pDefinitionManager->getDefinition( objectType );
	if (pDefinition == nullptr)
	{
		PyErr_Format( PyExc_TypeError,
			"Could not find a definition for \"%s\"\n", objectType );
		return nullptr;
	}

	ObjectHandle object = pDefinition->create();
	CHECK( object != nullptr );

	// TODO NGT-1052

	// Return none to pass the test
	Py_RETURN_NONE;
}


/**
 *	
 *	
 *	@param instance the reflected Python object to test.
 *	
 *	@throw TypeError when arguments cannot be parsed.
 *	@throw TypeError when the Python class cannot be converted.
 *	
 *	@return None.
 */
PyObject * parseArguments( PyObject * self,
	PyObject * args,
	PyObject * kw )
{
	PyObject * object = nullptr;

	static char *keywords [] = {
		"object",
		nullptr
	};

	if (!PyArg_ParseTupleAndKeywords( args, kw, "O", keywords, &object ))
	{
		// PyArg_ParseTupleAndKeywords sets error indicator
		return nullptr;
	}
	if (object == nullptr)
	{
		PyErr_Format( PyExc_TypeError,
			"Cannot test null object." );
		return nullptr;
	}

	return object;
}


void noneConversionTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ );
void numericConversionTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ );
void stringConversionTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ );
void childConversionTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ );
void listConversionTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ );
void tupleConversionTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ );
void dictConversionTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ );
void methodConversionTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ );
void newPropertyTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ );
void pathTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ );
void compareTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ );


/**
 *	Test converting a Python object to a reflected object.
 *	
 *	@param instance the reflected Python object to test.
 *	
 *	@throw TypeError when arguments cannot be parsed.
 *	@throw TypeError when the Python class cannot be converted.
 *	
 *	@return None.
 */
static PyObject * commonConversionTest(
	ReflectedPython::DefinedInstance & instance )
{
	if (g_module == nullptr)
	{
		PyErr_Format( PyExc_Exception,
			"Module is not loaded." );
		return nullptr;
	}
	const char * m_name = g_module->testName_;
	TestResult & result_ = g_module->result_;

	// Check that the Python object's definition is working
	// At the moment a different definition is made for each Python object
	// instance
	{
		const IClassDefinition & genericDefinition = *instance.getDefinition();

		const ClassDefinition * pGenericClassDefinition =
			dynamic_cast< const ClassDefinition * >( &genericDefinition );
		if (pGenericClassDefinition == nullptr)
		{
			PyErr_Format( PyExc_TypeError,
				"Failed to get correct definition." );
			return nullptr;
		}

		const IClassDefinitionDetails& details =
			pGenericClassDefinition->getDetails();

		const ReflectedPython::DefinitionDetails * pPythonDefinition =
			dynamic_cast< const ReflectedPython::DefinitionDetails * >( &details );
		if (pPythonDefinition == nullptr)
		{
			PyErr_Format( PyExc_TypeError,
				"Failed to get correct definition." );
			return nullptr;
		}
	}

	// Test getting properties from the instance
	// Using the Python object's definition
	noneConversionTest( instance, m_name, result_ );
	numericConversionTest( instance, m_name, result_ );
	stringConversionTest( instance, m_name, result_ );
	childConversionTest( instance, m_name, result_ );
	listConversionTest( instance, m_name, result_ );
	tupleConversionTest( instance, m_name, result_ );
	dictConversionTest( instance, m_name, result_ );
	methodConversionTest( instance, m_name, result_ );
	newPropertyTest( instance, m_name, result_ );
	pathTest( instance, m_name, result_ );
	compareTest( instance, m_name, result_ );

	// Return none to pass the test
	Py_RETURN_NONE;
}


void noneConversionTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ )
{
	// Convert Python None -> C++ nullptr
	{
		// @see Py_None
		void * noneType = nullptr;
		const bool setSuccess = instance.set< void * >(
			"noneTest", noneType );

		CHECK( setSuccess );

		void * noneResult;
		const bool getSuccess = instance.get< void * >(
			"noneTest", noneResult );

		CHECK( getSuccess );
		CHECK_EQUAL( noneType, noneResult );
	}
}


void numericConversionTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ )
{
	{
		// @see PyBoolObject
		const bool boolExpected = false;
		const bool setSuccess = instance.set< bool >( "boolTest", boolExpected );

		CHECK( setSuccess );

		bool boolResult = true;
		const bool getSuccess = instance.get< bool >( "boolTest", boolResult );

		CHECK( getSuccess );
		CHECK_EQUAL( boolExpected, boolResult );
	}
	{
		// @see PyIntObject
		const int intExpected = 2;
		const bool setSuccess = instance.set< int >( "intTest", intExpected );

		CHECK( setSuccess );

		int intResult = 1;
		const bool getSuccess = instance.get< int >( "intTest", intResult );

		CHECK( getSuccess );
		CHECK_EQUAL( intExpected, intResult );
	}
	// Check class properties
	{
		// @see PyIntObject
		const int classIntExpected = 2;
		const bool setSuccess = instance.set< int >( "classIntTest", classIntExpected );

		CHECK( setSuccess );

		int classIntResult = 1;
		const bool getSuccess = instance.get< int >( "classIntTest", classIntResult );

		CHECK( getSuccess );
		CHECK_EQUAL( classIntExpected, classIntResult );
	}
	{
		// @see PyLongObject
		const digit longExpected = 2;
		const bool setSuccess = instance.set< digit >( "longTest", longExpected );

		CHECK( setSuccess );

		digit longResult = 1;
		const bool getSuccess = instance.get< digit >( "longTest", longResult );

		CHECK( getSuccess );
		CHECK_EQUAL( longExpected, longResult );
	}
	{
		// @see PyFloatObject
		const double floatExpected = 2.0;
		const bool setSuccess = instance.set< double >( "floatTest", floatExpected );

		CHECK( setSuccess );

		double floatCheck = 1.0;
		const bool getSuccess = instance.get< double >( "floatTest", floatCheck );

		CHECK( getSuccess );
		// TODO direct floating point comparison is bad
		CHECK_EQUAL( floatExpected, floatCheck );
	}
	// TODO structs
	//{
	//	// @see PyComplexObject
	//	Py_complex complexExpected;
	//	complexExpected.real = 1.0;
	//	complexExpected.imag = 0.0;
	//	const bool setSuccess = instance.set< Py_complex >(
	//		"complexExpected", complexExpected );

	//	CHECK( setSuccess );

	//	Py_complex complexResult;
	//	complexResult.real = 0.0;
	//	complexResult.imag = 1.0;
	//	const bool getSuccess = instance.get< Py_complex >(
	//		"complexExpected", complexResult );

	//	CHECK( getSuccess );
	//	// TODO direct floating point comparison is bad
	//	CHECK( (complexExpected.real == complexResult.real) &&
	//		(complexExpected.imag == complexResult.imag) );
	//}
}


void stringConversionTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ )
{
	{
		// @see PyStringObject
		const std::string stringExpected = "String was set";
		const bool setSuccess = instance.set< std::string >(
			"stringTest", stringExpected );

		CHECK( setSuccess );

		std::string stringResult;
		const bool getSuccess = instance.get< std::string >(
			"stringTest", stringResult );

		CHECK( getSuccess );
		CHECK_EQUAL( stringExpected, stringResult );
	}
	{
		// @see PyUnicodeObject
		const std::wstring unicodeExpected = L"String was set";
		const bool setSuccess = instance.set< std::wstring >(
			"unicodeTest", unicodeExpected );
		CHECK( setSuccess );

		std::wstring unicodeResult = L"Fail";
		const bool getSuccess = instance.get< std::wstring >(
			"unicodeTest", unicodeResult );

		CHECK( getSuccess );
		// std::wstring is not compatible with CHECK_EQUAL
		CHECK( unicodeExpected == unicodeResult );
	}
}


void childConversionTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ )
{
	{
		// @see object in object
		const std::string childPropertyExpected = "Child";
		const bool setSuccess = instance.set< std::string >(
			"childTest.stringTest", childPropertyExpected );

		CHECK( setSuccess );

		std::string childPropertyResult;
		const bool getSuccess = instance.get< std::string >(
			"childTest.stringTest", childPropertyResult );

		CHECK( getSuccess );
		CHECK_EQUAL( childPropertyExpected, childPropertyResult );
	}
}


void resetList( ReflectedPython::DefinedInstance& instance,
	const size_t size,
	const char * m_name,
	TestResult & result_ )
{
	// Reset list in case another test above modified it
	std::vector< Variant > container;
	container.reserve( size );
	for (int i = 0; i < static_cast< int >( container.capacity() ); ++i)
	{
		container.emplace_back( i );
	}
	Collection listTest( container );
	const bool resetSuccess = instance.set< Collection >(
		"listTest", listTest );

	CHECK( resetSuccess );
}


void checkList( const Collection & listResult,
	size_t expectedSize,
	const char * m_name,
	TestResult & result_ )
{
	int i = 0;
	CHECK_EQUAL( expectedSize, listResult.size() );
	for (const auto & item : listResult)
	{
		int value = -1;
		const bool success = item.tryCast( value );
		CHECK( success );
		CHECK_EQUAL( i, value );
		++i;
	}
}


void listConversionTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ )
{
	{
		// @see PyListObject
		const size_t expectedSize = 10;
		resetList( instance, expectedSize, m_name, result_ );

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );

		CHECK( getSuccess );
		CHECK( listResult.canResize() );

		checkList( listResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyListObject
		// First element
		const int listExpected = 10;
		const bool setSuccess = instance.set< int >(
			"listTest[0]", listExpected );

		CHECK( setSuccess );

		int listResult = 0;
		const bool getSuccess = instance.get< int >(
			"listTest[0]", listResult );

		CHECK( getSuccess );
		CHECK_EQUAL( listExpected, listResult );
	}
	{
		// @see PyListObject
		// Second element
		const int listExpected = 11;
		const bool setSuccess = instance.set< int >(
			"listTest[1]", listExpected );

		CHECK( setSuccess );

		int listResult = 0;
		const bool getSuccess = instance.get< int >(
			"listTest[1]", listResult );

		CHECK( getSuccess );
		CHECK_EQUAL( listExpected, listResult );
	}
	{
		// @see PyListObject
		// Out-of-range
		const int listExpected = 11;
		const bool setSuccess = instance.set< int >(
			"listTest[999]", listExpected );

		CHECK( !setSuccess );

		int listResult = 0;
		const bool getSuccess = instance.get< int >(
			"listTest[999]", listResult );

		CHECK( !getSuccess );
	}
	{
		// @see PyListObject
		// Last item in list

		// Reset list in case another test above modified it
		const size_t expectedSize = 5;
		resetList( instance, expectedSize, m_name, result_ );

		const int listExpected = 11;
		const bool setSuccess = instance.set< int >(
			"listTest[-1]", listExpected );

		CHECK( setSuccess );

		{
			int listResult = 0;
			const bool getSuccess = instance.get< int >(
				"listTest[-1]", listResult );

			CHECK( getSuccess );
			CHECK_EQUAL( listExpected, listResult );
		}
		{
			int listResult = 0;
			const bool getSuccess = instance.get< int >(
				"listTest[4]", listResult );

			CHECK( getSuccess );
			CHECK_EQUAL( listExpected, listResult );
		}
	}
	{
		// @see PyListObject
		// Negative out-of-range

		// Reset list in case another test above modified it
		const size_t expectedSize = 5;
		resetList( instance, expectedSize, m_name, result_ );

		const int listExpected = 11;
		const bool setSuccess = instance.set< int >(
			"listTest[-100]", listExpected );

		CHECK( !setSuccess );

		int listResult = 0;
		const bool getSuccess = instance.get< int >(
			"listTest[-100]", listResult );

		CHECK( !getSuccess );
	}
	{
		//// @see PyListObject
		//// TODO NGT-1423 Slicing

		//// Reset list in case another test above modified it
		//const size_t expectedSize = 5;
		//resetList( instance, expectedSize, m_name, result_ );

		//// Set all items in the range 0-3
		//const int listExpected = 11;
		//const bool setSuccess = instance.set< int >(
		//	"listTest[0:3]", listExpected );

		//CHECK( setSuccess );

		//int listResult = 0;
		//const bool getSuccess = instance.get< int >(
		//	"listTest[2]", listResult );

		//CHECK( getSuccess );
		//CHECK_EQUAL( listExpected, listResult );
	}
	{
		//// @see PyListObject
		//// TODO NGT-1423 Slicing

		//// Reset list in case another test above modified it
		//const size_t expectedSize = 5;
		//resetList( instance, expectedSize, m_name, result_ );

		//// Set all items in the range 1-3, with a step of 1
		//// i.e. set 1, 2, 3
		//const int listExpected = 11;
		//const bool setSuccess = instance.set< int >(
		//	"listTest[1:3:1]", listExpected );

		//CHECK( setSuccess );

		//int listResult = 0;
		//const bool getSuccess = instance.get< int >(
		//	"listTest[2]", listResult );

		//CHECK( getSuccess );
		//CHECK_EQUAL( listExpected, listResult );
	}
	{
		//// @see PyListObject
		//// TODO NGT-1423 Slicing

		//// Reset list in case another test above modified it
		//const size_t expectedSize = 5;
		//resetList( instance, expectedSize, m_name, result_ );

		//// Set all items in the range first-last, with a step of 2
		//// i.e. set 0, 2, 4
		//const int listExpected = 11;
		//const bool setSuccess = instance.set< int >(
		//	"listTest[::2]", listExpected );

		//CHECK( setSuccess );

		//int listResult = 0;
		//const bool getSuccess = instance.get< int >(
		//	"listTest[2]", listResult );

		//CHECK( getSuccess );
		//CHECK_EQUAL( listExpected, listResult );
	}
	{
		// @see PyListObject
		// Append to end
		// Reset list in case another test above modified it
		const size_t originalSize = 5;
		resetList( instance, originalSize, m_name, result_ );

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );

		CHECK( getSuccess );
		CHECK( listResult.canResize() );
		
		const size_t insertionSize = 5;
		for (int i = 0; i < static_cast< int >( originalSize ); ++i)
		{
			Variant key( i + originalSize );
			auto insertionItr = listResult.insert( key );
			CHECK( insertionItr != listResult.end() );
			insertionItr.setValue( key );
		}

		const size_t expectedSize = (originalSize + insertionSize);
		checkList( listResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyListObject
		// Insert in middle
		// Reset list in case another test above modified it
		const size_t originalSize = 5;
		resetList( instance, originalSize, m_name, result_ );

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );

		CHECK( getSuccess );
		CHECK( listResult.canResize() );
		
		const int insertionPosition = 2;
		{
			Variant position( insertionPosition );
			auto insertionItr = listResult.insert( position );
			CHECK( insertionItr != listResult.end() );
			insertionItr.setValue( position );
		}

		{
			int i = 0;
			const size_t expectedSize = (originalSize + 1);
			CHECK_EQUAL( expectedSize, listResult.size() );
			for (const auto & item : listResult)
			{
				int value = -1;
				const bool success = item.tryCast( value );
				CHECK( success );
				if (i <= insertionPosition)
				{
					CHECK_EQUAL( i, value );
				}
				else
				{
					CHECK_EQUAL( i - 1, value );
				}
				++i;
			}
		}
	}
	{
		// @see PyListObject
		// Insert at start
		// Reset list in case another test above modified it
		const size_t originalSize = 5;
		resetList( instance, originalSize, m_name, result_ );

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );

		CHECK( getSuccess );
		CHECK( listResult.canResize() );
		
		const int insertionPosition = -100;
		{
			Variant position( insertionPosition );
			auto insertionItr = listResult.insert( position );
			CHECK( insertionItr != listResult.end() );
			insertionItr.setValue( position );
		}

		{
			int i = 0;
			const size_t expectedSize = (originalSize + 1);
			CHECK_EQUAL( expectedSize, listResult.size() );
			for (const auto & item : listResult)
			{
				int value = -1;
				const bool success = item.tryCast( value );
				CHECK( success );
				if (i == 0)
				{
					CHECK_EQUAL( insertionPosition, value );
				}
				else
				{
					CHECK_EQUAL( i - 1, value );
				}
				++i;
			}
		}
	}
	{
		// @see PyListObject
		// Erase existing item by key
		// Reset list in case another test above modified it
		const size_t originalSize = 5;
		resetList( instance, originalSize, m_name, result_ );

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );

		CHECK( getSuccess );

		const size_t erasureId = originalSize - 1;
		const size_t maxDigits = 10;
		char buffer[ maxDigits ];
		PlatformHelpers::sprintfSize_t(buffer, erasureId);
		Variant key( buffer );
		auto erasureCount = listResult.eraseKey( key );
		CHECK( erasureCount == 1 );

		const size_t expectedSize = originalSize - 1;
		checkList( listResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyListObject
		// Erase with invalid key
		// Reset list in case another test above modified it
		const size_t originalSize = 5;
		resetList( instance, originalSize, m_name, result_ );

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );

		CHECK( getSuccess );

		Variant key( "Invalid" );
		auto erasureCount = listResult.eraseKey( key );
		CHECK( erasureCount == 0 );

		const size_t expectedSize = originalSize;
		checkList( listResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyListObject
		// Erase existing item by iterator
		// Reset list in case another test above modified it
		const size_t originalSize = 5;
		resetList( instance, originalSize, m_name, result_ );

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );

		CHECK( getSuccess );

		const size_t erasureId = 2;
		{
			const size_t maxDigits = 10;
			char buffer[ maxDigits ];

			PlatformHelpers::sprintfSize_t(buffer, erasureId);
			const Variant key( buffer );
			auto itr = listResult.find( key );
			CHECK( itr != listResult.end() );
			auto erasureItr = listResult.erase( itr );
			CHECK( erasureItr != listResult.end() );
		}

		const size_t expectedSize = originalSize - 1;
		CHECK_EQUAL( expectedSize, listResult.size() );

		int i = 0;
		for (const auto & item : listResult)
		{
			const int expectedValue = (i < erasureId) ? i : i + 1;
			int value = -1;
			const bool success = item.tryCast( value );
			CHECK( success );
			CHECK_EQUAL( expectedValue, value );
			++i;
		}
	}
	{
		// @see PyListObject
		// Erase by invalid iterator
		// Reset list in case another test above modified it
		const size_t originalSize = 5;
		resetList( instance, originalSize, m_name, result_ );

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );

		CHECK( getSuccess );

		{
			auto itr = listResult.end();
			auto erasureItr = listResult.erase( itr );
			CHECK( erasureItr == listResult.end() );
		}

		const size_t expectedSize = originalSize;
		checkList( listResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyListObject
		// Erase existing item by iterator range
		// Reset list in case another test above modified it
		const size_t originalSize = 5;
		resetList( instance, originalSize, m_name, result_ );

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );

		CHECK( getSuccess );

		const size_t startId = 1;
		const size_t endId = 3;
		const Variant startKey( startId );
		const Variant endKey( endId );

		const auto startItr = listResult.find( startKey );
		CHECK( startItr != listResult.end() );

		const auto endItr = listResult.find( endKey );
		CHECK( endItr != listResult.end() );

		auto erasureItr = listResult.erase( startItr, endItr );
		CHECK( erasureItr != listResult.end() );
		{
			const int expectedPosition = 1;
			int value = -1;
			const bool success = erasureItr.key().tryCast( value );
			CHECK( success );
			CHECK_EQUAL( expectedPosition, value );
		}

		const size_t expectedSize = originalSize - (endId - startId);
		CHECK_EQUAL( expectedSize, listResult.size() );

		int i = 0;
		for (const auto & item : listResult)
		{
			if ((i >= startId) && (i < endId))
			{
				continue;
			}
			const int expectedValue = (i < startId) ? i : i + (endId - startId);
			int value = -1;
			const bool success = item.tryCast( value );
			CHECK( success );
			CHECK_EQUAL( expectedValue, value );
			++i;
		}
	}
	{
		// @see PyListObject
		// Erase existing item by iterator range of size 0
		// Reset list in case another test above modified it
		const size_t originalSize = 5;
		resetList( instance, originalSize, m_name, result_ );

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );

		CHECK( getSuccess );

		const size_t erasureId = 1;
		const Variant erasureKey( erasureId );

		const auto startItr = listResult.find( erasureKey );
		CHECK( startItr != listResult.end() );

		const auto endItr = startItr;

		auto erasureItr = listResult.erase( startItr, endItr );
		CHECK( erasureItr == listResult.end() );

		const size_t expectedSize = originalSize;
		checkList( listResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyListObject
		// Erase entire list
		// Reset list in case another test above modified it
		const size_t originalSize = 5;
		resetList( instance, originalSize, m_name, result_ );

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );

		CHECK( getSuccess );

		const size_t startId = 0;
		const size_t endId = originalSize;
		const Variant startKey( startId );
		const Variant endKey( endId );

		const auto startItr = listResult.find( startKey );
		CHECK( startItr != listResult.end() );

		const auto endItr = listResult.find( endKey );
		CHECK( endItr == listResult.end() );

		auto erasureItr = listResult.erase( startItr, endItr );
		CHECK( erasureItr == listResult.end() );

		CHECK( listResult.empty() );
	}
	{
		// @see PyListObject
		// Erase existing item by invalid range
		// Reset list in case another test above modified it
		const size_t originalSize = 5;
		resetList( instance, originalSize, m_name, result_ );

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );

		CHECK( getSuccess );

		const auto startItr = listResult.end();
		const auto endItr = listResult.end();

		const auto erasureItr = listResult.erase( startItr, endItr );
		CHECK( erasureItr == listResult.end() );

		const size_t expectedSize = originalSize;
		checkList( listResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyListObject
		// Get existing with operator[]
		// Reset list in case another test above modified it
		const size_t originalSize = 5;
		resetList( instance, originalSize, m_name, result_ );

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );

		CHECK( getSuccess );
		CHECK( listResult.canResize() );
		
		const int getPosition = 2;
		Variant position( getPosition );
		auto valueRef = listResult[ position ];

		int result = 0;
		const bool success = valueRef.tryCast< int >( result );
		CHECK( success );
		CHECK( result == getPosition );
	}
	{
		// @see PyListObject
		// Insert at end with operator[]
		// Reset list in case another test above modified it
		const size_t originalSize = 5;
		resetList( instance, originalSize, m_name, result_ );

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );

		CHECK( getSuccess );
		CHECK( listResult.canResize() );
		
		const int getPosition = originalSize;
		{
			Variant position( getPosition );
			auto valueRef = listResult[ position ];

			// Check it inserted None
			void * result = static_cast< void * >( &position );
			const bool success = valueRef.tryCast< void * >( result );
			CHECK( success );
			CHECK( result == nullptr );

			// Set value to int
			valueRef = getPosition;
		}

		const size_t expectedSize = (originalSize + 1);
		checkList( listResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyListObject
		// Insert at start with operator[]
		// Reset list in case another test above modified it
		const size_t originalSize = 5;
		resetList( instance, originalSize, m_name, result_ );

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );

		CHECK( getSuccess );
		CHECK( listResult.canResize() );
		
		const int getPosition = -static_cast< int >( originalSize ) - 1;
		{
			Variant position( getPosition );
			auto valueRef = listResult[ position ];

			// Check it inserted None
			void * result = static_cast< void * >( &position );
			const bool success = valueRef.tryCast< void * >( result );
			CHECK( success );
			CHECK( result == nullptr );

			// Set value to int
			valueRef = getPosition;
		}

		{
			int i = 0;
			const size_t expectedSize = (originalSize + 1);
			CHECK_EQUAL( expectedSize, listResult.size() );
			for (const auto & item : listResult)
			{
				int value = -1;
				const bool success = item.tryCast( value );
				CHECK( success );
				if (i == 0)
				{
					CHECK_EQUAL( getPosition, value );
				}
				else
				{
					CHECK_EQUAL( i - 1, value );
				}
				++i;
			}
		}
	}
	{
		// @see PyListObject
		// List containing different types
		const size_t originalSize = 5;
		{
			std::vector< Variant > container;
			container.reserve( originalSize );
			container.emplace_back( 0 );
			container.emplace_back( 1 );
			container.emplace_back( 2 );
			container.emplace_back( "Hello" );
			container.emplace_back( "World" );
			Collection listTest( container );
			const bool resetSuccess = instance.set< Collection >(
				"listTest", listTest );

			CHECK( resetSuccess );
		}

		Collection listResult;
		const bool getSuccess = instance.get< Collection >(
			"listTest", listResult );
		CHECK( getSuccess );
		CHECK( listResult.canResize() );
		
		const size_t expectedSize = originalSize;
		CHECK_EQUAL( expectedSize, listResult.size() );
		int i = 0;
		for (const auto & item : listResult)
		{
			if (i < 3)
			{
				int value = -1;
				const bool success = item.tryCast( value );
				CHECK( success );
				CHECK_EQUAL( i, value );
			}
			else if (i == 3)
			{
				std::string value;
				const bool success = item.tryCast( value );
				CHECK( success );
				CHECK_EQUAL( "Hello", value );
			}
			else if (i == 4)
			{
				std::string value;
				const bool success = item.tryCast( value );
				CHECK( success );
				CHECK_EQUAL( "World", value );
			}
			++i;
		}
	}
	{
		// @see PyListObject
		// List in list
		{
			const size_t originalSize = 4;
			std::vector< Variant > container1;
			container1.reserve( originalSize );
			container1.emplace_back( 0 );
			container1.emplace_back( 1 );
			container1.emplace_back( 2 );
			container1.emplace_back( 3 );
			std::vector< Variant > container2;
			container2.reserve( originalSize );
			container2.emplace_back( container1 );
			container2.emplace_back( 1 );
			container2.emplace_back( 2 );
			container2.emplace_back( 3 );
			Collection listTest( container2 );
			const bool resetSuccess = instance.set< Collection >(
				"listTest", listTest );

			CHECK( resetSuccess );
		}

		const int listExpected = 10;
		const bool setSuccess = instance.set< int >(
			"listTest[0][1]", listExpected );

		CHECK( setSuccess );

		int listResult = 0;
		const bool getSuccess = instance.get< int >(
			"listTest[0][1]", listResult );

		CHECK( getSuccess );
		CHECK_EQUAL( listExpected, listResult );
	}
}


template< size_t size >
void resetTuple( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ )
{
	// Reset list in case another test above modified it
	std::array< Variant, size > container;
	for (int i = 0; i < static_cast< int >( container.max_size() ); ++i)
	{
		container[ i ] = i;
	}
	Collection tupleTest( container );
	const bool resetSuccess = instance.set< Collection >(
		"tupleTest", tupleTest );

	CHECK( resetSuccess );
}


void checkTuple( const Collection & tupleResult,
	size_t expectedSize,
	const char * m_name,
	TestResult & result_ )
{
	int i = 0;
	CHECK_EQUAL( expectedSize, tupleResult.size() );
	for (const auto & item : tupleResult)
	{
		int value = -1;
		const bool success = item.tryCast( value );
		CHECK( success );
		CHECK_EQUAL( i, value );
		++i;
	}
}


void tupleConversionTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ )
{
	{
		// @see PyTupleObject
		const size_t expectedSize = 10;
		resetTuple< expectedSize >( instance, m_name, result_ );

		Collection tupleResult;
		const bool getSuccess = instance.get< Collection >(
			"tupleTest", tupleResult );

		CHECK( getSuccess );
		CHECK( !tupleResult.canResize() );

		checkTuple( tupleResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyTupleObject
		// First element
		const int tupleExpected = 10;
		const bool setSuccess = instance.set< int >(
			"tupleTest[0]", tupleExpected );

		CHECK( setSuccess );

		int tupleResult = 0;
		const bool getSuccess = instance.get< int >(
			"tupleTest[0]", tupleResult );

		CHECK( getSuccess );
		CHECK_EQUAL( tupleExpected, tupleResult );
	}
	{
		// @see PyTupleObject
		// Second element
		const int tupleExpected = 11;
		const bool setSuccess = instance.set< int >(
			"tupleTest[1]", tupleExpected );

		CHECK( setSuccess );

		int tupleResult = 0;
		const bool getSuccess = instance.get< int >(
			"tupleTest[1]", tupleResult );

		CHECK( getSuccess );
		CHECK_EQUAL( tupleExpected, tupleResult );
	}
	{
		// @see PyTupleObject
		// Out-of-range
		const int tupleExpected = 11;
		const bool setSuccess = instance.set< int >(
			"tupleTest[999]", tupleExpected );

		CHECK( !setSuccess );

		int tupleResult = 0;
		const bool getSuccess = instance.get< int >(
			"tupleTest[999]", tupleResult );

		CHECK( !getSuccess );
	}
	{
		// @see PyTupleObject
		// Last item in tuple

		// Reset tuple in case another test above modified it
		const size_t expectedSize = 5;
		resetTuple< expectedSize >( instance, m_name, result_ );

		const int tupleExpected = 11;
		const bool setSuccess = instance.set< int >(
			"tupleTest[-1]", tupleExpected );

		CHECK( setSuccess );

		{
			int tupleResult = 0;
			const bool getSuccess = instance.get< int >(
				"tupleTest[-1]", tupleResult );

			CHECK( getSuccess );
			CHECK_EQUAL( tupleExpected, tupleResult );
		}
		{
			int tupleResult = 0;
			const bool getSuccess = instance.get< int >(
				"tupleTest[4]", tupleResult );

			CHECK( getSuccess );
			CHECK_EQUAL( tupleExpected, tupleResult );
		}
	}
	{
		// @see PyTupleObject
		// Negative out-of-range

		// Reset tuple in case another test above modified it
		const size_t expectedSize = 5;
		resetTuple< expectedSize >( instance, m_name, result_ );

		const int tupleExpected = 11;
		const bool setSuccess = instance.set< int >(
			"tupleTest[-100]", tupleExpected );

		CHECK( !setSuccess );

		int tupleResult = 0;
		const bool getSuccess = instance.get< int >(
			"tupleTest[-100]", tupleResult );

		CHECK( !getSuccess );
	}
	{
		//// @see PyTupleObject
		//// TODO NGT-1423 Slicing

		//// Reset tuple in case another test above modified it
		//const size_t expectedSize = 5;
		//resetTuple< expectedSize >( instance, m_name, result_ );

		//// Set all items in the range 0-3
		//const int tupleExpected = 11;
		//const bool setSuccess = instance.set< int >(
		//	"tupleTest[0:3]", tupleExpected );

		//CHECK( setSuccess );

		//int tupleResult = 0;
		//const bool getSuccess = instance.get< int >(
		//	"tupleTest[2]", tupleResult );

		//CHECK( getSuccess );
		//CHECK_EQUAL( tupleExpected, tupleResult );
	}
	{
		//// @see PyTupleObject
		//// TODO NGT-1423 Slicing

		//// Reset tuple in case another test above modified it
		//const size_t expectedSize = 5;
		//resetTuple< expectedSize >( instance, m_name, result_ );

		//// Set all items in the range 1-3, with a step of 1
		//// i.e. set 1, 2, 3
		//const int tupleExpected = 11;
		//const bool setSuccess = instance.set< int >(
		//	"tupleTest[1:3:1]", tupleExpected );

		//CHECK( setSuccess );

		//int tupleResult = 0;
		//const bool getSuccess = instance.get< int >(
		//	"tupleTest[2]", tupleResult );

		//CHECK( getSuccess );
		//CHECK_EQUAL( tupleExpected, tupleResult );
	}
	{
		//// @see PyTupleObject
		//// TODO NGT-1423 Slicing

		//// Reset tuple in case another test above modified it
		//const size_t expectedSize = 5;
		//resetTuple< expectedSize >( instance, m_name, result_ );

		//// Set all items in the range first-last, with a step of 2
		//// i.e. set 0, 2, 4
		//const int tupleExpected = 11;
		//const bool setSuccess = instance.set< int >(
		//	"tupleTest[::2]", tupleExpected );

		//CHECK( setSuccess );

		//int tupleResult = 0;
		//const bool getSuccess = instance.get< int >(
		//	"tupleTest[2]", tupleResult );

		//CHECK( getSuccess );
		//CHECK_EQUAL( tupleExpected, tupleResult );
	}
	{
		// @see PyTupleObject
		// Append to end
		// Reset tuple in case another test above modified it
		const size_t originalSize = 5;
		resetTuple< originalSize >( instance, m_name, result_ );

		Collection tupleResult;
		const bool getSuccess = instance.get< Collection >(
			"tupleTest", tupleResult );

		CHECK( getSuccess );
		CHECK( !tupleResult.canResize() );
		
		Variant position( originalSize + 1 );
		auto insertionItr = tupleResult.insert( position );
		CHECK( insertionItr == tupleResult.end() );

		const size_t expectedSize = originalSize;
		checkTuple( tupleResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyTupleObject
		// Insert in middle
		// Reset tuple in case another test above modified it
		const size_t originalSize = 5;
		resetTuple< originalSize >( instance, m_name, result_ );

		Collection tupleResult;
		const bool getSuccess = instance.get< Collection >(
			"tupleTest", tupleResult );

		CHECK( getSuccess );
		CHECK( !tupleResult.canResize() );
		
		const int insertionPosition = 2;
		Variant position( insertionPosition );
		auto insertionItr = tupleResult.insert( position );
		CHECK( insertionItr == tupleResult.end() );

		const size_t expectedSize = originalSize;
		checkTuple( tupleResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyTupleObject
		// Insert at start
		// Reset tuple in case another test above modified it
		const size_t originalSize = 5;
		resetTuple< originalSize >( instance, m_name, result_ );

		Collection tupleResult;
		const bool getSuccess = instance.get< Collection >(
			"tupleTest", tupleResult );

		CHECK( getSuccess );
		CHECK( !tupleResult.canResize() );
		
		const int insertionPosition = -100;
		Variant position( insertionPosition );
		auto insertionItr = tupleResult.insert( position );
		CHECK( insertionItr == tupleResult.end() );

		const size_t expectedSize = originalSize;
		checkTuple( tupleResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyTupleObject
		// Erase existing item by key
		// Reset tuple in case another test above modified it
		const size_t originalSize = 5;
		resetTuple< originalSize >( instance, m_name, result_ );

		Collection tupleResult;
		const bool getSuccess = instance.get< Collection >(
			"tupleTest", tupleResult );

		CHECK( getSuccess );

		const size_t erasureId = originalSize - 1;
		Variant key( erasureId );
		auto erasureCount = tupleResult.eraseKey( key );
		CHECK( erasureCount == 0 );

		const size_t expectedSize = originalSize;
		checkTuple( tupleResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyTupleObject
		// Erase existing item by iterator
		// Reset tuple in case another test above modified it
		const size_t originalSize = 5;
		resetTuple< originalSize >( instance, m_name, result_ );

		Collection tupleResult;
		const bool getSuccess = instance.get< Collection >(
			"tupleTest", tupleResult );

		CHECK( getSuccess );

		const size_t erasureId = 2;
		{
			const Variant key( erasureId );
			auto itr = tupleResult.find( key );
			CHECK( itr != tupleResult.end() );
			auto erasureItr = tupleResult.erase( itr );
			CHECK( erasureItr == tupleResult.end() );
		}

		const size_t expectedSize = originalSize;
		checkTuple( tupleResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyTupleObject
		// Erase by invalid iterator
		// Reset tuple in case another test above modified it
		const size_t originalSize = 5;
		resetTuple< originalSize >( instance, m_name, result_ );

		Collection tupleResult;
		const bool getSuccess = instance.get< Collection >(
			"tupleTest", tupleResult );

		CHECK( getSuccess );

		{
			auto itr = tupleResult.end();
			auto erasureItr = tupleResult.erase( itr );
			CHECK( erasureItr == tupleResult.end() );
		}

		const size_t expectedSize = originalSize;
		checkTuple( tupleResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyTupleObject
		// Erase existing item by iterator range
		// Reset tuple in case another test above modified it
		const size_t originalSize = 5;
		resetTuple< originalSize >( instance, m_name, result_ );

		Collection tupleResult;
		const bool getSuccess = instance.get< Collection >(
			"tupleTest", tupleResult );

		CHECK( getSuccess );

		const size_t startId = 1;
		const size_t endId = 3;
		const Variant startKey( startId );
		const Variant endKey( endId );

		const auto startItr = tupleResult.find( startKey );
		CHECK( startItr != tupleResult.end() );

		const auto endItr = tupleResult.find( endKey );
		CHECK( endItr != tupleResult.end() );

		auto erasureItr = tupleResult.erase( startItr, endItr );
		CHECK( erasureItr == tupleResult.end() );

		const size_t expectedSize = originalSize;
		checkTuple( tupleResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyTupleObject
		// Get existing with operator[]
		// Reset tuple in case another test above modified it
		const size_t originalSize = 5;
		resetTuple< originalSize >( instance, m_name, result_ );

		Collection tupleResult;
		const bool getSuccess = instance.get< Collection >(
			"tupleTest", tupleResult );

		CHECK( getSuccess );
		CHECK( !tupleResult.canResize() );
		
		const int getPosition = 2;
		Variant position( getPosition );
		auto valueRef = tupleResult[ position ];

		int result = 0;
		const bool success = valueRef.tryCast< int >( result );
		CHECK( success );
		CHECK( result == getPosition );
	}
	{
		// @see PyTupleObject
		// Insert at end with operator[]
		// Reset tuple in case another test above modified it
		const size_t originalSize = 5;
		resetTuple< originalSize >( instance, m_name, result_ );

		Collection tupleResult;
		const bool getSuccess = instance.get< Collection >(
			"tupleTest", tupleResult );

		CHECK( getSuccess );
		CHECK( !tupleResult.canResize() );
		
		const int getPosition = originalSize;
		{
			Variant position( getPosition );
			auto valueRef = tupleResult[ position ];

			// Check it returns end (did not insert)
			CHECK( Variant( valueRef ).isVoid() );

			// Set value to int
			valueRef = getPosition;

			// Check it returns end (did not set)
			CHECK( Variant( valueRef ).isVoid() );
		}

		const size_t expectedSize = originalSize;
		checkTuple( tupleResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyTupleObject
		// Insert at start with operator[]
		// Reset tuple in case another test above modified it
		const size_t originalSize = 5;
		resetTuple< originalSize >( instance, m_name, result_ );

		Collection tupleResult;
		const bool getSuccess = instance.get< Collection >(
			"tupleTest", tupleResult );

		CHECK( getSuccess );
		CHECK( !tupleResult.canResize() );
		
		const int getPosition = -static_cast< int >( originalSize ) - 1;
		{
			Variant position( getPosition );
			auto valueRef = tupleResult[ position ];

			// Check it returns end (did not insert)
			CHECK( Variant( valueRef ).isVoid() );

			// Set value to int
			valueRef = getPosition;

			// Check it returns end (did not set)
			CHECK( Variant( valueRef ).isVoid() );
		}

		const size_t expectedSize = originalSize;
		checkTuple( tupleResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyTupleObject
		// Tuple containing different types
		const size_t originalSize = 5;
		{
			std::array< Variant, originalSize > container;
			container[ 0 ] = 0;
			container[ 1 ] = 1;
			container[ 2 ] = 2;
			container[ 3 ] = "Hello";
			container[ 4 ] = "World";
			Collection tupleTest( container );
			const bool resetSuccess = instance.set< Collection >(
				"tupleTest", tupleTest );

			CHECK( resetSuccess );
		}

		Collection tupleResult;
		const bool getSuccess = instance.get< Collection >(
			"tupleTest", tupleResult );
		CHECK( getSuccess );
		CHECK( !tupleResult.canResize() );
		
		const size_t expectedSize = originalSize;
		CHECK_EQUAL( expectedSize, tupleResult.size() );
		int i = 0;
		for (const auto & item : tupleResult)
		{
			if (i < 3)
			{
				int value = -1;
				const bool success = item.tryCast( value );
				CHECK( success );
				CHECK_EQUAL( i, value );
			}
			else if (i == 3)
			{
				std::string value;
				const bool success = item.tryCast( value );
				CHECK( success );
				CHECK_EQUAL( "Hello", value );
			}
			else if (i == 4)
			{
				std::string value;
				const bool success = item.tryCast( value );
				CHECK( success );
				CHECK_EQUAL( "World", value );
			}
			++i;
		}
	}
	{
		// @see PyTupleObject
		// Tuple in tuple
		{
			const size_t originalSize = 4;
			std::array< Variant, originalSize > container1;
			container1[ 0 ] = 0;
			container1[ 1 ] = 1;
			container1[ 2 ] = 2;
			container1[ 3 ] = 3;
			std::array< Variant, originalSize > container2;
			container2[ 0 ] = container1;
			container2[ 1 ] = 1;
			container2[ 2 ] = 2;
			container2[ 3 ] = 3;
			Collection tupleTest( container2 );
			const bool resetSuccess = instance.set< Collection >(
				"tupleTest", tupleTest );

			CHECK( resetSuccess );
		}

		const int tupleExpected = 10;
		const bool setSuccess = instance.set< int >(
			"listTest[0][1]", tupleExpected );

		CHECK( setSuccess );

		int tupleResult = 0;
		const bool getSuccess = instance.get< int >(
			"listTest[0][1]", tupleResult );

		CHECK( getSuccess );
		CHECK_EQUAL( tupleExpected, tupleResult );
	}
}


void resetDict( ReflectedPython::DefinedInstance & instance,
	size_t size,
	const char * m_name,
	TestResult & result_ )
{
	// Reset dict in case another test above modified it
	std::map< std::string, int > container;
	const size_t maxDigits = 10;
	char buffer[ maxDigits ];
	for (int i = 0; i < static_cast< int >( size ); ++i)
	{
		sprintf( buffer, "%d", i );
		container[ buffer ] = i;
	}
	Collection dictTest( container );
	const bool resetSuccess = instance.set< Collection >(
		"dictTest", dictTest );

	CHECK( resetSuccess );
}


void checkDict( const Collection & dictResult,
	size_t expectedSize,
	const char * m_name,
	TestResult & result_ )
{
	const size_t maxDigits = 10;
	char buffer[ maxDigits ];
	std::string bufferStr;
	bufferStr.reserve( maxDigits );

	// Note that Python dictionaries are unordered
	// So it needs to lookup by key so that the expected value is known
	CHECK_EQUAL( expectedSize, dictResult.size() );
	for (int i = 0; i < static_cast< int >( expectedSize ); ++i)
	{
		sprintf( buffer, "%d", i );
		bufferStr = buffer;
		auto itr = dictResult.find( bufferStr );
		CHECK( itr != dictResult.end() );

		std::string key;
		const bool keySuccess = itr.key().tryCast( key );
		CHECK( keySuccess );
		CHECK_EQUAL( bufferStr, key );

		int value = -1;
		const bool valueSuccess = itr.value().tryCast( value );
		CHECK( valueSuccess );
		CHECK_EQUAL( i, value );
	}
}


void dictConversionTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ )
{
	{
		// @see PyDictObject
		const size_t expectedSize = 10;
		resetDict( instance, expectedSize, m_name, result_ );

		Collection dictResult;
		const bool getSuccess = instance.get< Collection >(
			"dictTest", dictResult );

		CHECK( getSuccess );
		CHECK( dictResult.canResize() );

		checkDict( dictResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyDictObject
		// Unordered iteration
		// Reset dict in case another test above modified it
		const size_t originalSize = 5;
		resetDict( instance, originalSize, m_name, result_ );

		Collection dictResult;
		const bool getSuccess = instance.get< Collection >(
			"dictTest", dictResult );

		CHECK( getSuccess );

		const size_t expectedSize = originalSize;
		CHECK_EQUAL( expectedSize, dictResult.size() );

		std::vector< int > expectedValues( expectedSize );
		int i = 0;
		std::generate( expectedValues.begin(),
			expectedValues.end(),
			[&i]() -> int {
				return i++;
			}
		);
		for (auto itr = dictResult.cbegin(); itr != dictResult.cend(); ++itr)
		{
			std::string key;
			const bool keySuccess = itr.key().tryCast( key );
			CHECK( keySuccess );
			CHECK_EQUAL( 1, key.size() );
			const auto index = atol( key.c_str() );

			const auto foundItr = std::find( expectedValues.cbegin(),
				expectedValues.cend(),
				index );
			CHECK( foundItr != expectedValues.end() );
			expectedValues.erase( foundItr );

			int value = -1;
			const bool valueSuccess = itr.value().tryCast( value );
			CHECK( valueSuccess );
			CHECK_EQUAL( index, value );
		}
	}
	{
		// @see PyDictObject
		// Invalid key
		// Reset dict in case another test above modified it
		const size_t originalSize = 5;
		resetDict( instance, originalSize, m_name, result_ );

		Collection dictResult;
		const bool getSuccess = instance.get< Collection >(
			"dictTest", dictResult );

		CHECK( getSuccess );

		const char * invalidKey = "Invalid";
		Variant testKey( invalidKey );
		auto foundItr = dictResult.find( testKey );
		CHECK( foundItr == dictResult.end() );

		{
			const Variant resultKeyVariant = foundItr.key();
			std::string resultKey;
			const bool success = resultKeyVariant.tryCast< std::string >( resultKey );
			CHECK( success );
			CHECK_EQUAL( invalidKey, resultKey );
		}
		CHECK( foundItr.value().isVoid() );
	}
	{
		// @see PyDictObject
		// Insert new item
		// Reset dict in case another test above modified it
		const size_t originalSize = 5;
		resetDict( instance, originalSize, m_name, result_ );

		Collection dictResult;
		const bool getSuccess = instance.get< Collection >(
			"dictTest", dictResult );

		CHECK( getSuccess );

		const size_t insertionId = originalSize;
		const size_t maxDigits = 10;
		char buffer[ maxDigits ];
		PlatformHelpers::sprintfSize_t(buffer, insertionId);
		Variant key( buffer );
		auto insertionItr = dictResult.insert( key );
		CHECK( insertionItr != dictResult.end() );
		
		{
			// Check it inserted None
			const auto insertionResult = (*insertionItr);
			void * result = static_cast< void * >( &buffer );
			const bool success = insertionResult.tryCast< void * >( result );
			CHECK( success );
			CHECK( result == nullptr );
		}

		{
			// Set value to int
			const bool success = insertionItr.setValue( insertionId );
			CHECK( success );
		}

		const size_t expectedSize = originalSize + 1;
		checkDict( dictResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyDictObject
		// Erase existing item by key
		// Reset dict in case another test above modified it
		const size_t originalSize = 5;
		resetDict( instance, originalSize, m_name, result_ );

		Collection dictResult;
		const bool getSuccess = instance.get< Collection >(
			"dictTest", dictResult );

		CHECK( getSuccess );

		const size_t erasureId = originalSize - 1;
		const size_t maxDigits = 10;
		char buffer[ maxDigits ];
		PlatformHelpers::sprintfSize_t(buffer, erasureId);
		Variant key( buffer );
		auto erasureCount = dictResult.eraseKey( key );
		CHECK( erasureCount == 1 );

		const size_t expectedSize = originalSize - 1;
		checkDict( dictResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyDictObject
		// Erase with invalid key
		// Reset dict in case another test above modified it
		const size_t originalSize = 5;
		resetDict( instance, originalSize, m_name, result_ );

		Collection dictResult;
		const bool getSuccess = instance.get< Collection >(
			"dictTest", dictResult );

		CHECK( getSuccess );

		Variant key( "Invalid" );
		auto erasureCount = dictResult.eraseKey( key );
		CHECK( erasureCount == 0 );

		const size_t expectedSize = originalSize;
		checkDict( dictResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyDictObject
		// Erase existing item by iterator
		// Reset dict in case another test above modified it
		const size_t originalSize = 5;
		resetDict( instance, originalSize, m_name, result_ );

		Collection dictResult;
		const bool getSuccess = instance.get< Collection >(
			"dictTest", dictResult );

		CHECK( getSuccess );

		const size_t erasureId = 2;
		{
			const size_t maxDigits = 10;
			char buffer[ maxDigits ];
			PlatformHelpers::sprintfSize_t( buffer, erasureId );
			const Variant key( buffer );
			auto itr = dictResult.find( key );
			CHECK( itr != dictResult.end() );
			auto erasureItr = dictResult.erase( itr );
			CHECK( erasureItr != dictResult.end() );
		}

		const size_t expectedSize = originalSize - 1;
		{
			const size_t maxDigits = 10;
			char buffer[ maxDigits ];
			std::string bufferStr;
			bufferStr.reserve( maxDigits );

			CHECK_EQUAL( expectedSize, dictResult.size() );
			for (int i = 0; i < expectedSize; ++i)
			{
				if (i == erasureId)
				{
					continue;
				}
				const int expectedValue = i;
				sprintf( buffer, "%d", expectedValue );
				bufferStr = buffer;
				auto itr = dictResult.find( bufferStr );
				CHECK( itr != dictResult.end() );

				std::string key;
				const bool keySuccess = itr.key().tryCast( key );
				CHECK( keySuccess );
				CHECK_EQUAL( bufferStr, key );

				int value = -1;
				const bool valueSuccess = itr.value().tryCast( value );
				CHECK( valueSuccess );
				CHECK_EQUAL( expectedValue, value );
			}
		}
	}
	{
		// @see PyDictObject
		// Erase by invalid iterator
		// Reset dict in case another test above modified it
		const size_t originalSize = 5;
		resetDict( instance, originalSize, m_name, result_ );

		Collection dictResult;
		const bool getSuccess = instance.get< Collection >(
			"dictTest", dictResult );

		CHECK( getSuccess );

		{
			auto itr = dictResult.end();
			auto erasureItr = dictResult.erase( itr );
			CHECK( erasureItr == dictResult.end() );
		}

		const size_t expectedSize = originalSize;
		checkDict( dictResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyDictObject
		// Erase existing item by iterator range
		// Reset dict in case another test above modified it
		const size_t originalSize = 5;
		resetDict( instance, originalSize, m_name, result_ );

		Collection dictResult;
		const bool getSuccess = instance.get< Collection >(
			"dictTest", dictResult );

		CHECK( getSuccess );

		const size_t startId = 1;
		const size_t endId = 3;
		const size_t maxDigits = 10;
		char buffer[ maxDigits ];
		PlatformHelpers::sprintfSize_t( buffer, startId );
		const Variant startKey( buffer );
		PlatformHelpers::sprintfSize_t(buffer, endId );
		const Variant endKey( buffer );

		const auto startItr = dictResult.find( startKey );
		CHECK( startItr != dictResult.end() );

		const auto endItr = dictResult.find( endKey );
		CHECK( endItr != dictResult.end() );

		auto erasureItr = dictResult.erase( startItr, endItr );
		CHECK( erasureItr != dictResult.end() );

		const size_t expectedSize = originalSize - (endId - startId);
		{
			CHECK_EQUAL( expectedSize, dictResult.size() );
			// Expected dict? - unordered so don't know which elements erased
		}
	}
	{
		// @see PyDictObject
		// Erase existing item by iterator range of size 1
		// Reset dict in case another test above modified it
		const size_t originalSize = 5;
		resetDict( instance, originalSize, m_name, result_ );

		Collection dictResult;
		const bool getSuccess = instance.get< Collection >(
			"dictTest", dictResult );

		CHECK( getSuccess );

		const size_t erasureId = 1;
		const size_t maxDigits = 10;
		char buffer[ maxDigits ];
		PlatformHelpers::sprintfSize_t(buffer, erasureId );
		const Variant erasureKey( buffer );

		const auto startItr = dictResult.find( erasureKey );
		CHECK( startItr != dictResult.end() );

		const auto endItr = startItr;

		auto erasureItr = dictResult.erase( startItr, endItr );
		CHECK( erasureItr == dictResult.end() );

		const size_t expectedSize = originalSize;
		checkDict( dictResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyDictObject
		// Erase existing item by invalid range
		// Reset dict in case another test above modified it
		const size_t originalSize = 5;
		resetDict( instance, originalSize, m_name, result_ );

		Collection dictResult;
		const bool getSuccess = instance.get< Collection >(
			"dictTest", dictResult );

		CHECK( getSuccess );

		const auto startItr = dictResult.end();
		const auto endItr = dictResult.end();

		const auto erasureItr = dictResult.erase( startItr, endItr );
		CHECK( erasureItr == dictResult.end() );

		const size_t expectedSize = originalSize;
		checkDict( dictResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyDictObject
		// Erase entire map
		// Reset dict in case another test above modified it
		const size_t originalSize = 5;
		resetDict( instance, originalSize, m_name, result_ );

		Collection dictResult;
		const bool getSuccess = instance.get< Collection >(
			"dictTest", dictResult );

		CHECK( getSuccess );

		const auto startItr = dictResult.begin();
		const auto endItr = dictResult.end();
		auto erasureItr = dictResult.erase( startItr, endItr );
		CHECK( erasureItr == dictResult.end() );

		CHECK( dictResult.empty() );
	}
	{
		// @see PyDictObject
		// Get existing with operator[]
		// Reset dict in case another test above modified it
		const size_t originalSize = 5;
		resetDict( instance, originalSize, m_name, result_ );

		Collection dictResult;
		const bool getSuccess = instance.get< Collection >(
			"dictTest", dictResult );

		CHECK( getSuccess );
		
		const int getId = 2;
		const size_t maxDigits = 10;
		char buffer[ maxDigits ];
		sprintf( buffer, "%d", getId );
		const Variant expectedKey( buffer );
		auto valueRef = dictResult[ expectedKey ];

		int result = 0;
		const bool success = valueRef.tryCast< int >( result );
		CHECK( success );
		CHECK( result == getId );
	}
	{
		// @see PyDictObject
		// Insert new item with operator[]
		// Reset dict in case another test above modified it
		const size_t originalSize = 5;
		resetDict( instance, originalSize, m_name, result_ );

		Collection dictResult;
		const bool getSuccess = instance.get< Collection >(
			"dictTest", dictResult );

		CHECK( getSuccess );
		
		const int getId = originalSize;
		const size_t maxDigits = 10;
		char buffer[ maxDigits ];
		sprintf( buffer, "%d", getId );
		const Variant expectedKey( buffer );
		auto valueRef = dictResult[ expectedKey ];

		// Check it inserted None
		void * result = static_cast< void * >( &dictResult );
		const bool success = valueRef.tryCast< void * >( result );
		CHECK( success );
		CHECK( result == nullptr );

		// Set value to int
		valueRef = getId;

		const size_t expectedSize = originalSize + 1;
		checkDict( dictResult, expectedSize, m_name, result_ );
	}
	{
		// @see PyDictObject
		// Dict in dict
		{
			std::map< int, Variant > container1;
			container1[ 0 ] = 0;
			container1[ 1 ] = 1;
			container1[ 2 ] = 2;
			container1[ 3 ] = 3;
			std::map< int, Variant > container2;
			container2[ 0 ] = container1;
			container2[ 1 ] = 1;
			container2[ 2 ] = 2;
			container2[ 3 ] = 3;
			Collection tupleTest( container2 );
			const bool resetSuccess = instance.set< Collection >(
				"dictTest", tupleTest );

			CHECK( resetSuccess );
		}

		const int dictExpected = 10;
		const bool setSuccess = instance.set< int >(
			"dictTest[0][1]", dictExpected );

		CHECK( setSuccess );

		int dictResult = 0;
		const bool getSuccess = instance.get< int >(
			"dictTest[0][1]", dictResult );

		CHECK( getSuccess );
		CHECK_EQUAL( dictExpected, dictResult );
	}
	{
		// @see PyDictObject
		// Dict containing different key types
		// TODO NGT-1332
		//const size_t originalSize = 5;
		//{
		//	std::map< Variant, Variant > container;
		//	const size_t maxDigits = 10;
		//	char buffer[ maxDigits ];
		//	sprintf( buffer, "%d", 0 );
		//	container[ buffer ] = 0;
		//	sprintf( buffer, "%d", 1 );
		//	container[ buffer ] = 1;
		//	sprintf( buffer, "%d", 2 );
		//	container[ buffer ] = 2;
		//	container[ 3 ] = "Hello";
		//	container[ 4 ] = "World";
		//	Collection dictTest( container );
		//	const bool resetSuccess = instance.set< Collection >(
		//		"dictTest", dictTest );

		//	CHECK( resetSuccess );
		//}

		//Collection dictResult;
		//const bool getSuccess = instance.get< Collection >(
		//	"dictTest", dictResult );
		//CHECK( getSuccess );

		//{
		//	const size_t maxDigits = 10;
		//	char buffer[ maxDigits ];
		//	std::string bufferStr;
		//	bufferStr.reserve( maxDigits );

		//	const size_t expectedSize = originalSize;
		//	CHECK_EQUAL( expectedSize, dictResult.size() );
		//	for (int i = 0; i < expectedSize; ++i)
		//	{
		//		if (i < 3)
		//		{
		//			sprintf( buffer, "%d", i );
		//			bufferStr = buffer;
		//			auto itr = dictResult.find( bufferStr );
		//			CHECK( itr != dictResult.end() );

		//			std::string key;
		//			const bool keySuccess = itr.key().tryCast( key );
		//			CHECK( keySuccess );
		//			CHECK_EQUAL( bufferStr, key );

		//			int value = -1;
		//			const bool valueSuccess = itr.value().tryCast( value );
		//			CHECK( valueSuccess );
		//			CHECK_EQUAL( i, value );
		//		}
		//		else if (i == 3)
		//		{
		//			auto itr = dictResult.find( i );
		//			CHECK( itr != dictResult.end() );

		//			int key = -1;
		//			const bool keySuccess = itr.key().tryCast( key );
		//			CHECK( keySuccess );
		//			CHECK_EQUAL( i, key );

		//			std::string value;
		//			const bool valueSuccess = itr.value().tryCast( value );
		//			CHECK( valueSuccess );
		//			CHECK_EQUAL( "Hello", value );
		//		}
		//		else if (i == 4)
		//		{
		//			auto itr = dictResult.find( i );
		//			CHECK( itr != dictResult.end() );

		//			int key = -1;
		//			const bool keySuccess = itr.key().tryCast( key );
		//			CHECK( keySuccess );
		//			CHECK_EQUAL( i, key );

		//			std::string value;
		//			const bool valueSuccess = itr.value().tryCast( value );
		//			CHECK( valueSuccess );
		//			CHECK_EQUAL( "World", value );
		//		}
		//	}
		//}
	}
	{
	//	// @see PyDictObject
	//	// TODO NGT-1332 Dict invalid key type
	}
}


void methodConversionTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ )
{
	{
		ReflectedMethodParameters parameters;
		parameters.push_back( Variant( "was run" ) );
		const Variant result = instance.invoke( "noneTest", parameters );
		CHECK( result.isVoid() );
	}
	{
		const auto property = instance.findProperty( "noneTest" );
		CHECK( property.isValid() );
		if (!property.isValid())
		{
			return;
		}
		const auto baseProperty = property.getProperty();
		CHECK( baseProperty != nullptr );
		if (baseProperty == nullptr)
		{
			return;
		}

		CHECK( !baseProperty->isMethod() );
		CHECK( baseProperty->isValue() );

		const auto parameterCount = baseProperty->parameterCount();
		const size_t expected = 0;
		CHECK_EQUAL( expected, parameterCount );
	}
	{
		ReflectedMethodParameters parameters;
		parameters.push_back( Variant( "was run" ) );
		const Variant result = instance.invoke( "methodTest", parameters );

		const std::string returnValue = result.value< std::string >();
		CHECK_EQUAL( "Method test was run", returnValue );
	}
	{
		const auto property = instance.findProperty( "methodTest" );
		CHECK( property.isValid() );
		if (!property.isValid())
		{
			return;
		}
		const auto baseProperty = property.getProperty();
		CHECK( baseProperty != nullptr );
		if (baseProperty == nullptr)
		{
			return;
		}

		CHECK( baseProperty->isMethod() );
		CHECK( baseProperty->isValue() );

		const auto parameterCount = baseProperty->parameterCount();
		const size_t expected = 1;
		CHECK_EQUAL( expected, parameterCount );
	}
	{
		ReflectedMethodParameters parameters;
		parameters.push_back( Variant( "was run" ) );
		const Variant result = instance.invoke( "classMethodTest", parameters );

		const std::string returnValue = result.value< std::string >();
		CHECK_EQUAL( "Class method test was run", returnValue );
	}
	{
		const auto property = instance.findProperty( "classMethodTest" );
		CHECK( property.isValid() );
		if (!property.isValid())
		{
			return;
		}
		const auto baseProperty = property.getProperty();
		CHECK( baseProperty != nullptr );
		if (baseProperty == nullptr)
		{
			return;
		}

		CHECK( baseProperty->isMethod() );
		CHECK( baseProperty->isValue() );

		const auto parameterCount = baseProperty->parameterCount();
		const size_t expected = 1;
		CHECK_EQUAL( expected, parameterCount );
	}
	{
		ReflectedMethodParameters parameters;
		parameters.push_back( Variant( "was run" ) );
		const Variant result = instance.invoke( "staticMethodTest", parameters );

		const std::string returnValue = result.value< std::string >();
		CHECK_EQUAL( "Static method test was run", returnValue );
	}
	{
		const auto property = instance.findProperty( "staticMethodTest" );
		CHECK( property.isValid() );
		if (!property.isValid())
		{
			return;
		}
		const auto baseProperty = property.getProperty();
		CHECK( baseProperty != nullptr );
		if (baseProperty == nullptr)
		{
			return;
		}

		CHECK( baseProperty->isMethod() );
		CHECK( baseProperty->isValue() );

		const auto parameterCount = baseProperty->parameterCount();
		const size_t expected = 1;
		CHECK_EQUAL( expected, parameterCount );
	}
	{
		ReflectedMethodParameters parameters;
		parameters.push_back( Variant( "was run" ) );
		const Variant result = instance.invoke( "functionTest1", parameters );

		const std::string returnValue = result.value< std::string >();
		CHECK_EQUAL( "Function test was run", returnValue );
	}
	{
		const auto property = instance.findProperty( "functionTest1" );
		CHECK( property.isValid() );
		if (!property.isValid())
		{
			return;
		}
		const auto baseProperty = property.getProperty();
		CHECK( baseProperty != nullptr );
		if (baseProperty == nullptr)
		{
			return;
		}

		CHECK( baseProperty->isMethod() );
		CHECK( baseProperty->isValue() );

		const auto parameterCount = baseProperty->parameterCount();
		const size_t expected = 1;
		CHECK_EQUAL( expected, parameterCount );
	}
	{
		ReflectedMethodParameters parameters;
		parameters.push_back( Variant( "was run" ) );
		const Variant result = instance.invoke( "functionTest2", parameters );

		const std::string returnValue = result.value< std::string >();
		CHECK_EQUAL( "Callable class test was run", returnValue );
	}
	{
		const auto property = instance.findProperty( "functionTest2" );
		CHECK( property.isValid() );
		if (!property.isValid())
		{
			return;
		}
		const auto baseProperty = property.getProperty();
		CHECK( baseProperty != nullptr );
		if (baseProperty == nullptr)
		{
			return;
		}

		CHECK( baseProperty->isMethod() );
		CHECK( baseProperty->isValue() );

		const auto parameterCount = baseProperty->parameterCount();
		const size_t expected = 1;
		CHECK_EQUAL( expected, parameterCount );
	}
	{
		ReflectedMethodParameters parameters;
		parameters.push_back( Variant( "was run" ) );
		const Variant result = instance.invoke( "functionTest3", parameters );

		const std::string returnValue = result.value< std::string >();
		CHECK_EQUAL( "Callable class test was run", returnValue );
	}
	{
		const auto property = instance.findProperty( "functionTest3" );
		CHECK( property.isValid() );
		if (!property.isValid())
		{
			return;
		}
		const auto baseProperty = property.getProperty();
		CHECK( baseProperty != nullptr );
		if (baseProperty == nullptr)
		{
			return;
		}

		CHECK( baseProperty->isMethod() );
		CHECK( baseProperty->isValue() );

		const auto parameterCount = baseProperty->parameterCount();
		const size_t expected = 1;
		CHECK_EQUAL( expected, parameterCount );
	}
	{
		ReflectedMethodParameters parameters;
		parameters.push_back( Variant( "was run" ) );
		const Variant result = instance.invoke( "ConstructorTest1", parameters );

		// __init__() should return None
		const void * returnValue = result.value< void * >();
		CHECK( returnValue == nullptr );
	}
	{
		const auto property = instance.findProperty( "ConstructorTest1" );
		CHECK( property.isValid() );
		if (!property.isValid())
		{
			return;
		}
		const auto baseProperty = property.getProperty();
		CHECK( baseProperty != nullptr );
		if (baseProperty == nullptr)
		{
			return;
		}

		CHECK( baseProperty->isMethod() );
		CHECK( baseProperty->isValue() );

		const auto parameterCount = baseProperty->parameterCount();
		const size_t expected = 1;
		CHECK_EQUAL( expected, parameterCount );
	}
	{
		ReflectedMethodParameters parameters;
		const Variant result = instance.invoke( "ConstructorTest2", parameters );

		// __init__() should return None
		const void * returnValue = result.value< void * >();
		CHECK( returnValue == nullptr );
	}
	{
		const auto property = instance.findProperty( "ConstructorTest2" );
		CHECK( property.isValid() );
		if (!property.isValid())
		{
			return;
		}
		const auto baseProperty = property.getProperty();
		CHECK( baseProperty != nullptr );
		if (baseProperty == nullptr)
		{
			return;
		}

		CHECK( baseProperty->isMethod() );
		CHECK( baseProperty->isValue() );

		const auto parameterCount = baseProperty->parameterCount();
		const size_t expected = 0;
		CHECK_EQUAL( expected, parameterCount );
	}
}


void newPropertyTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ )
{
	{
		// @see Py_None
		void * noneType = nullptr;
		const bool setSuccess = instance.set< void * >(
			"newPropertyTest", noneType );

		CHECK( setSuccess );

		void * noneResult;
		const bool getSuccess = instance.get< void * >(
			"newPropertyTest", noneResult );

		CHECK( getSuccess );
		CHECK_EQUAL( noneType, noneResult );
	}

	{
		// @see PyIntObject
		const int intExpected = 2;
		const bool setSuccess = instance.set< int >( "newIntTest", intExpected );

		CHECK( setSuccess );

		int intResult = 1;
		const bool getSuccess = instance.get< int >( "newIntTest", intResult );

		CHECK( getSuccess );
		CHECK_EQUAL( intExpected, intResult );
	}
}


/**
 *	Access "childTest", then "collectionName" in two steps.
 */
void getCollectionPath1( const ReflectedPython::DefinedInstance & instance,
	Collection & outCollection,
	const char * collectionName,
	const char * m_name,
	TestResult & result_ )
{
	ObjectHandle childHandle;
	const bool getChildSuccess = instance.get< ObjectHandle >(
		"childTest", childHandle );

	CHECK( getChildSuccess );
	CHECK( childHandle.isValid() );
	if (!childHandle.isValid())
	{
		return;
	}

	const auto pInstance = childHandle.getBase< ReflectedPython::DefinedInstance >();
	CHECK( pInstance != nullptr );
	if (pInstance == nullptr)
	{
		return;
	}
	const auto & child = (*pInstance);

	const bool getCollectionSuccess = child.get< Collection >(
		collectionName, outCollection );

	CHECK( getCollectionSuccess );
	CHECK( outCollection.isValid() );
}


/**
 *	Access "childTest.collectionName" in one step.
 */
void getCollectionPath2( const ReflectedPython::DefinedInstance & instance,
	Collection & outCollection,
	const char * collectionName,
	const char * m_name,
	TestResult & result_ )
{
	std::string pathName = "childTest";
	pathName += IClassDefinition::DOT_OPERATOR;
	pathName += collectionName;
	const bool getCollectionSuccess = instance.get< Collection >(
		pathName.c_str(), outCollection );

	CHECK( getCollectionSuccess );
	CHECK( outCollection.isValid() );
}


/**
 *	Test if items in a sequence have the correct paths.
 */
void checkSequencePaths( const ReflectedPython::DefinedInstance & instance,
	const Collection & collection,
	const char * collectionName,
	const char * m_name,
	TestResult & result_ )
{
	auto expectedKey = 0;
	for (auto itr = collection.cbegin();
		itr != collection.cend();
		++itr, ++expectedKey)
	{
		int key = -1;
		const auto keySuccess = itr.key().tryCast( key );
		CHECK( keySuccess );
		CHECK_EQUAL( expectedKey, key );

		ObjectHandle value;
		const auto valueSuccess = itr.value().tryCast( value );

		const auto pValueInstance = value.getBase< ReflectedPython::DefinedInstance >();
		CHECK( pValueInstance != nullptr );
		if (pValueInstance == nullptr)
		{
			return;
		}
		const auto & valueInstance = (*pValueInstance);

		const auto & valueRoot = valueInstance.root();
		CHECK_EQUAL( &instance, &valueRoot );

		const auto & valueFullPath = valueInstance.fullPath();

		std::string expectedValueFullPath = "childTest";
		expectedValueFullPath += IClassDefinition::DOT_OPERATOR;
		expectedValueFullPath += collectionName;
		expectedValueFullPath += IClassDefinition::INDEX_OPEN;
		expectedValueFullPath += std::to_string( expectedKey );
		expectedValueFullPath += IClassDefinition::INDEX_CLOSE;
		CHECK_EQUAL( expectedValueFullPath, valueFullPath );

		int valueValue = -1;
		valueInstance.get< int >( "value", valueValue );
		CHECK_EQUAL( valueValue, expectedKey );
	}
}


/**
 *	Test if items in a dictionary have the correct paths.
 */
void checkMappingPaths( const ReflectedPython::DefinedInstance & instance,
	const Collection & collection,
	const char * collectionName,
	const char * m_name,
	TestResult & result_ )
{
	for (auto itr = collection.cbegin(); itr != collection.cend(); ++itr)
	{
		ObjectHandle key;
		const auto keySuccess = itr.key().tryCast( key );

		const auto pKeyInstance = key.getBase< ReflectedPython::DefinedInstance >();
		CHECK( pKeyInstance != nullptr );
		if (pKeyInstance == nullptr)
		{
			return;
		}
		const auto & keyInstance = (*pKeyInstance);

		const auto & keyRoot = keyInstance.root();
		CHECK_EQUAL( &keyInstance, &keyRoot );

		const auto & keyFullPath = keyInstance.fullPath();
		const std::string expectedKeyFullPath =
			keyInstance.pythonObject().str( PyScript::ScriptErrorPrint() ).c_str();
		CHECK_EQUAL( expectedKeyFullPath, keyFullPath );

		ObjectHandle value;
		const auto valueSuccess = itr.value().tryCast( value );

		const auto pValueInstance = value.getBase< ReflectedPython::DefinedInstance >();
		CHECK( pValueInstance != nullptr );
		if (pValueInstance == nullptr)
		{
			return;
		}
		const auto & valueInstance = (*pValueInstance);

		const auto & valueRoot = valueInstance.root();
		CHECK_EQUAL( &instance, &valueRoot );

		const auto & valueFullPath = valueInstance.fullPath();

		std::string expectedValueFullPath = "childTest";
		expectedValueFullPath += IClassDefinition::DOT_OPERATOR;
		expectedValueFullPath += collectionName;
		expectedValueFullPath += IClassDefinition::INDEX_OPEN;
		expectedValueFullPath += expectedKeyFullPath;
		expectedValueFullPath += IClassDefinition::INDEX_CLOSE;
		CHECK_EQUAL( expectedValueFullPath, valueFullPath );
	}
}


void pathTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ )
{
	// Access root (no path)
	{
		const auto & root = instance.root();
		CHECK_EQUAL( &instance, &root );

		const auto & fullPath = instance.fullPath();
		const char * expectedFullPath = "";
		CHECK_EQUAL( expectedFullPath, fullPath );
	}

	Collection collection;

	// Access "childTest", then "listTest"
	getCollectionPath1( instance, collection, "listTest", m_name, result_ );
	checkSequencePaths( instance, collection, "listTest", m_name, result_ );

	// Access "childTest.listTest"
	getCollectionPath2( instance, collection, "listTest", m_name, result_ );
	checkSequencePaths( instance, collection, "listTest", m_name, result_ );
	
	// Access "childTest", then "tupleTest"
	getCollectionPath1( instance, collection, "tupleTest", m_name, result_ );
	checkSequencePaths( instance, collection, "tupleTest", m_name, result_ );

	// Access "childTest.tupleTest"
	getCollectionPath2( instance, collection, "tupleTest", m_name, result_ );
	checkSequencePaths( instance, collection, "tupleTest", m_name, result_ );
	
	// Access "childTest", then "dictTest"
	getCollectionPath1( instance, collection, "dictTest", m_name, result_ );
	checkMappingPaths( instance, collection, "dictTest", m_name, result_ );

	// Access "childTest.dictTest"
	getCollectionPath2( instance, collection, "dictTest", m_name, result_ );
	checkMappingPaths( instance, collection, "dictTest", m_name, result_ );
}


void compareTest( ReflectedPython::DefinedInstance & instance,
	const char * m_name,
	TestResult & result_ )
{
	// Access object that cannot be compared to other objects
	{
		ObjectHandle result;
		const bool getSuccess = instance.get< ObjectHandle >(
			"badComparison", result );

		CHECK( getSuccess );
		CHECK( result.isValid() );
		CHECK( result.getBase< ReflectedPython::DefinedInstance >() != nullptr );
	}
}


/**
 *	Tests for converting an old-style Python class to a reflected object.

 *	@param self the reflection module.
 *	@param args first argument must be a Python class.
 *		e.g. reflection.conversionTest(testClass)
 *	@param argument keywords
 *		e.g. reflection.create(object=testClass)
 *	
 *	@throw TypeError when arguments cannot be parsed.
 *	@throw TypeError when the Python class cannot be converted.
 *	
 *	@return None.
 */
static PyObject * py_oldStyleConversionTest( PyObject * self,
	PyObject * args,
	PyObject * kw )
{
	if (g_module == nullptr)
	{
		PyErr_Format( PyExc_Exception,
			"Module is not loaded." );
		return nullptr;
	}
	const char * m_name = g_module->testName_;
	TestResult & result_ = g_module->result_;

	auto pyObject = parseArguments( self, args, kw );
	if (pyObject == nullptr)
	{
		// parseArguments sets the error indicator
		return nullptr;
	}
	PyScript::ScriptObject scriptObject( pyObject );

	ObjectHandle parentHandle;
	const char * childPath = "";
	ObjectHandle handle = ReflectedPython::DefinedInstance::findOrCreate(
		g_module->context_,
		scriptObject,
		parentHandle,
		childPath );
	auto pInstance = static_cast< ReflectedPython::DefinedInstance * >( handle.data() );
	assert( pInstance != nullptr );
	auto & instance = (*pInstance);

	auto pCommonResult = commonConversionTest( instance );
	if (pCommonResult == nullptr)
	{
		return pCommonResult;
	}

	auto definitionManager = g_module->context_.queryInterface<IDefinitionManager>();
	CHECK( definitionManager != nullptr );

	// @see types.ClassType for expectedType.
	auto checkObjectType = [&]( const char* attribute, const char* expectedTypeBase )
	{
		ObjectHandle handle;
		const bool getSuccess = instance.get<ObjectHandle>( attribute, handle );
		CHECK( getSuccess );
		
		auto definition = handle.getDefinition( *definitionManager );
		CHECK( definition != nullptr );

		auto actualType = definition->getName();

		const auto & details = static_cast< const ReflectedPython::DefinitionDetails & >(
			definition->getDetails() );
		std::string expectedType = expectedTypeBase;
		expectedType += " at ";
		expectedType += std::to_string( details.object().id().asUnsignedLongLong(
			PyScript::ScriptErrorRetain() ) );

		// Check for overflow
		CHECK( !PyScript::Script::hasError() );
		PyScript::Script::clearError();

		CHECK_EQUAL( expectedType, actualType );
	};

	// Convert Python types -> C++ TypeIds
	checkObjectType( "typeTest1", "__builtin__.classobj" );
	checkObjectType( "typeTest2", "__builtin__.type" );
	checkObjectType( "classTest1", "python27_test.OldClassTest" );
	checkObjectType( "classTest2", "python27_test.OldClassTest" );
	checkObjectType( "instanceTest", "__builtin__.instance" );
	
	// Convert Python type <- C++ TypeId
	{
		auto typeConverters = g_module->context_.queryInterface< PythonType::Converters >();
		CHECK( typeConverters != nullptr );
		if (typeConverters == nullptr)
		{
			Py_RETURN_NONE;
		}

		Variant intType;
		PyScript::ScriptType scriptObject( &PyInt_Type, PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

		ObjectHandle parent;
		const char * childPath = "";
		bool success = typeConverters->toVariant( scriptObject, intType, parent, childPath );
		CHECK( success );

		success = instance.set( "typeTest1", intType );
		CHECK( success );

		checkObjectType( "typeTest1", "__builtin__.int" );
	}
	
	Py_RETURN_NONE;
}


/**
 *	Tests for converting a new-style Python class to a reflected object.

 *	@param self the reflection module.
 *	@param args first argument must be a Python class.
 *		e.g. reflection.conversionTest(testClass)
 *	@param argument keywords
 *		e.g. reflection.create(object=testClass)
 *	
 *	@throw TypeError when arguments cannot be parsed.
 *	@throw TypeError when the Python class cannot be converted.
 *	
 *	@return None.
 */
static PyObject * py_newStyleConversionTest( PyObject * self,
	PyObject * args,
	PyObject * kw )
{
	if (g_module == nullptr)
	{
		PyErr_Format( PyExc_Exception,
			"Module is not loaded." );
		return nullptr;
	}
	const char * m_name = g_module->testName_;
	TestResult & result_ = g_module->result_;

	auto pyObject = parseArguments( self, args, kw );
	if (pyObject == nullptr)
	{
		// parseArguments sets the error indicator
		return nullptr;
	}
	PyScript::ScriptObject scriptObject( pyObject );

	ObjectHandle parentHandle;
	const char * childPath = "";
	ObjectHandle handle = ReflectedPython::DefinedInstance::findOrCreate(
		g_module->context_,
		scriptObject,
		parentHandle,
		childPath );
	auto pInstance = static_cast< ReflectedPython::DefinedInstance * >( handle.data() );
	assert( pInstance != nullptr );
	auto & instance = (*pInstance);

	auto pCommonResult = commonConversionTest( instance );
	if (pCommonResult == nullptr)
	{
		return pCommonResult;
	}

	auto definitionManager = g_module->context_.queryInterface<IDefinitionManager>();
	CHECK( definitionManager != nullptr );

	// @see types.ClassType for expectedType.
	auto checkObjectType = [&]( const char* attribute, const char* expectedTypeBase )
	{
		ObjectHandle handle;
		const bool getSuccess = instance.get<ObjectHandle>( attribute, handle );
		CHECK( getSuccess );

		auto definition = handle.getDefinition( *definitionManager );
		CHECK( definition != nullptr );

		auto actualType = definition->getName();

		const auto & details = static_cast< const ReflectedPython::DefinitionDetails & >(
			definition->getDetails() );
		std::string expectedType = expectedTypeBase;
		expectedType += " at ";
		expectedType += std::to_string( details.object().id().asUnsignedLongLong(
			PyScript::ScriptErrorRetain() ) );

		// Check for overflow
		assert( !PyScript::Script::hasError() );
#if defined( _DEBUG )
		PyScript::Script::clearError();
#endif // defined( _DEBUG )

		CHECK_EQUAL( expectedType, actualType );
	};

	// Convert Python types -> C++ TypeIds
	checkObjectType( "typeTest1", "__builtin__.type" );
	checkObjectType( "typeTest2", "__builtin__.type" );
	checkObjectType( "classTest1", "python27_test.NewClassTest" );
	checkObjectType( "classTest2", "python27_test.NewClassTest" );
	checkObjectType( "instanceTest", "python27_test.NewClassTest" );

	// Convert Python type <- C++ TypeId
	{
		auto typeConverters = g_module->context_.queryInterface< PythonType::Converters >();
		CHECK( typeConverters != nullptr );
		if (typeConverters == nullptr)
		{
			Py_RETURN_NONE;
		}

		Variant intType;
		PyScript::ScriptType scriptObject( &PyInt_Type, PyScript::ScriptObject::FROM_BORROWED_REFERENCE );

		ObjectHandle parent;
		const char * childPath = "";
		bool success = typeConverters->toVariant( scriptObject, intType, parent, childPath );
		CHECK( success );

		success = instance.set( "typeTest1", intType );
		CHECK( success );

		checkObjectType( "typeTest1", "__builtin__.int" );
	}

	{
		// @see property() builtin, @property decorator
		const std::string stringTest = "String was set";
		const bool setSuccess = instance.set< std::string >(
			"readOnlyPropertyTest1", stringTest );

		CHECK( !setSuccess );

		const std::string expectedString = "Read-only Property";
		std::string stringResult;
		const bool getSuccess = instance.get< std::string >(
			"readOnlyPropertyTest1", stringResult );

		CHECK( getSuccess );
		CHECK_EQUAL( expectedString, stringResult );
	}

	{
		// @see property() builtin, @property decorator
		const std::string stringTest = "String was set";
		const bool setSuccess = instance.set< std::string >(
			"readOnlyPropertyTest2", stringTest );

		CHECK( !setSuccess );

		const std::string expectedString = "Read-only Property";
		std::string stringResult;
		const bool getSuccess = instance.get< std::string >(
			"readOnlyPropertyTest2", stringResult );

		CHECK( getSuccess );
		CHECK_EQUAL( expectedString, stringResult );
	}

	{
		// @see descriptors __get__ and __set__
		const std::string stringExpected = "String was set";
		const bool setSuccess = instance.set< std::string >(
			"descriptorTest", stringExpected );

		CHECK( setSuccess );

		std::string stringResult;
		const bool getSuccess = instance.get< std::string >(
			"descriptorTest", stringResult );

		CHECK( getSuccess );
		CHECK_EQUAL( stringExpected, stringResult );
	}

	Py_RETURN_NONE;
}


} // namespace


ReflectionTestModule::ReflectionTestModule( IComponentContext & context,
	const char * testName,
	TestResult & result )
	: context_( context )
	, testName_( testName )
	, result_( result )
{
	g_module = this;

	const char * m_name = testName_;

	CHECK( Py_IsInitialized() );
	if (Py_IsInitialized())
	{
		static PyMethodDef s_methods[] =
		{
			{
				"create",
				reinterpret_cast< PyCFunction >( &py_create ),
				METH_VARARGS|METH_KEYWORDS,
				"Create C++ reflected object and return it to Python"
			},
			{
				"oldStyleConversionTest",
				reinterpret_cast< PyCFunction >( &py_oldStyleConversionTest ),
				METH_VARARGS|METH_KEYWORDS,
				"Inspect an old-style Python class using the reflection system"
			},
			{
				"newStyleConversionTest",
				reinterpret_cast< PyCFunction >( &py_newStyleConversionTest ),
				METH_VARARGS|METH_KEYWORDS,
				"Inspect a new-style Python class using the reflection system"
			},
			{ nullptr, nullptr, 0, nullptr }
		};

		PyObject *m = Py_InitModule( "reflectiontest", s_methods );
		CHECK( m != nullptr );
	}
}


ReflectionTestModule::~ReflectionTestModule()
{
	g_module = nullptr;
}
} // end namespace wgt
