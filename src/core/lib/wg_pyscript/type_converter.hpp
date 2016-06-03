#pragma once
#ifndef TYPE_CONVERTER_HPP
#define TYPE_CONVERTER_HPP


#include <cstdint>
#include <map>
#include <string>
#include <vector>


namespace wgt
{
namespace PyScript
{

class PyObjectPtr;


/**
 *	Collection of helper functions for converting between C++ and Python types.
 *	@see //lib/pyscript/script.cpp
 */
namespace Script
{
	int setData( PyObject * pObj, 
		bool & rVal, const char * varName = "" );
	int setData( PyObject * pObj, 
		int  & rVal, const char * varName = "" );
	int setData( PyObject * pObj, unsigned int & rVal, const char * varName = "" );
	int setData( PyObject * pObj, long & rVal, const char * varName = "" );

	int setData( PyObject * pObj, 
		float & rVal, const char * varName = "" );
	int setData( PyObject * pObj, double & rVal, const char * varName = "" );
	int setData( PyObject * pObj, int64_t & rVal, const char * varName = "" );
	int setData( PyObject * pObj, uint64_t & rVal, const char * varName = "" );
	int setData( PyObject * pObj, PyObject * & rVal,
		const char * varName = "" );
	int setData( PyObject * pObj, 
		PyObjectPtr & rPyObject,	const char * varName = "" );
	int setData( PyObject * pObj, std::string & rString,
		const char * varName = "" );
	int setData( PyObject * pObj, std::wstring & rString,
		const char * varName = "" );

	PyObject * getData( const bool data );
	PyObject * getData( const int data );
	PyObject * getData( const unsigned int data );
	PyObject * getData( const long data );

	PyObject * getData( const float data );
	PyObject * getData( const double data );
	PyObject * getData( const int64_t data );
	PyObject * getData( const uint64_t data );

	PyObject * getData( const PyObject * data );
	PyObject * getData( const PyObjectPtr & data );
	PyObject * getData( const std::string & data );
	PyObject * getData( const std::wstring & data );
	PyObject * getData( const char * data );
	PyObject * getData( const wchar_t * data );

#define INT_ACCESSOR( INPUT_TYPE, COMMON_TYPE )					\
	inline PyObject * getData( const INPUT_TYPE data )			\
		{ return getData( COMMON_TYPE( data ) ); }				\
	inline int setData( PyObject * pObject, INPUT_TYPE & rInt,	\
						const char * varName = "" )				\
		{														\
			COMMON_TYPE value;									\
			int result = setData( pObject, value, varName );	\
			rInt = INPUT_TYPE( value );							\
			if (rInt != value )									\
			{													\
				PyErr_SetString( PyExc_TypeError, 				\
					"Integer is out of range" );				\
																\
				return -1;										\
			}													\
			return result;										\
		}


	INT_ACCESSOR( int8_t,  int );
	INT_ACCESSOR( int16_t, int );

	INT_ACCESSOR( uint8_t,  int );
	INT_ACCESSOR( uint16_t, int );


	/**
	 *	setData function for directly-addressable sequences (vectors, strings).
	 *	Lists would require a push_back-based implementation.
	 */
	template <class T, class SEQ> int setDataSequence( PyObject * pObj,
		SEQ & res, const char * varName )
	{
		if (!PySequence_Check( pObj ))
		{
			PyErr_Format( PyExc_TypeError, "%s must be set to a sequence of %s",
				varName, typeid(T).name() );
			return -1;
		}
		std::string eltVarName = varName; eltVarName += " element";
		Py_ssize_t sz = PySequence_Size( pObj );
		res.resize( sz );
		for (Py_ssize_t i = 0; i < sz; ++i)
		{
			PyObjectPtr pItem( PySequence_GetItem( pObj, i ), true );
			if (setData( pItem.get(), res[i], eltVarName.c_str() ) != 0)
			{
				return -1;
			}
		}
		return 0;
	}

	/// setData for vectors
	template <class T, class A> int setData( PyObject * pObj,
		std::vector<T,A> & res, const char * varName = "" )
	{
		return setDataSequence<T>( pObj, res, varName );
	}

	/// setData for basic_strings
	template <class C, class Tr, class A> int setData( PyObject * pObj,
		std::basic_string<C,Tr,A> & res, const char * varName = "" )
	{
		return setDataSequence<C>( pObj, res, varName );
	}

	/**
	 *	setData function for mappings (maps, multimaps).
	 */
	template <class K, class T, class MAP> int setDataMapping( PyObject * pObj,
		MAP & res, const char * varName )
	{
		if (!PyDict_Check( pObj ))	// using PyMapping API would be expensive
		{
			PyErr_Format( PyExc_TypeError, "%s must be set to a dict of %s: %s",
				varName, typeid(K).name(), typeid(T).name() );
			return -1;
		}
		std::string keyVarName = varName;
		std::string valueVarName = keyVarName;
		keyVarName += " key";
		valueVarName += " value";

		res.clear();
		Py_ssize_t pos = 0;
		PyObject * pKey, * pValue;
		while (PyDict_Next( pObj, &pos, &pKey, &pValue ))
		{
			std::pair<K,T> both;
			if (setData( pKey, both.first, keyVarName.c_str() ) != 0) return -1;
			if (setData( pValue, both.second, valueVarName.c_str() ) != 0) return -1;
			res.insert( both );
		}
		return 0;
	}

	// setData for maps
	template <class K, class T, class C, class A> int setData( PyObject * pObj,
		std::map<K,T,C,A> & res, const char * varName = "" )
	{
		return setDataMapping<K,T>( pObj, res, varName );
	}

	// setData for multimaps
	template <class K, class T, class C, class A> int setData( PyObject * pObj,
		std::multimap<K,T,C,A> & res, const char * varName = "" )
	{
		return setDataMapping<K,T>( pObj, res, varName );
	}

} // namespace Script


} // namespace PyScript
} // end namespace wgt
#endif // TYPE_CONVERTER_HPP
