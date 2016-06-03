// py_script_object.ipp

/**
 *	This method calls a method on a ScriptObject
 *
 *	@param methodName	The method to call
 *	@param args			The arguments to call the method with
 *	@param errorHandler The type of error handling to use if this method
 *		fails
 *	@param allowNullMethod	True if this method can not exist, false otherwise.
 *	@return				The resulting ScriptObject
 */
template <class ERROR_HANDLER>
inline ScriptObject ScriptObject::callMethod( const char * methodName, 
		const ScriptArgs & args,
		const ERROR_HANDLER & errorHandler, 
		bool allowNullMethod /* = false */ ) const
{
	PyErr_Clear();

	ScriptObject method = this->getAttribute( methodName,
		ScriptErrorRetain() );

	if (!method)
	{
		if (allowNullMethod)
		{
			Script::clearError();
		}
		else
		{
			errorHandler.handleError();
		}

		return ScriptObject();
	}

	return method.callFunction( args, errorHandler );
}


/**
 *	This method calls a method on a ScriptObject
 *
 *	@param methodName	The method to call
 *	@param errorHandler The type of error handling to use if this method
 *		fails
 *	@param allowNullMethod	True if this method can not exist, false otherwise.
 *	@return				The resulting ScriptObject
 */
template <class ERROR_HANDLER>
inline ScriptObject ScriptObject::callMethod( const char * methodName,
		const ERROR_HANDLER & errorHandler, 
		bool allowNullMethod /* = false */ ) const
{
	return this->callMethod( methodName, ScriptArgs::none(),
			errorHandler, allowNullMethod );
}


/**
 *	This method calls the ScriptObject function
 *
 *	@param args			The arguments to call the method with
 *	@param kwargs		The named arguments to call the method with
 *	@param errorHandler The type of error handling to use if this method
 *		fails
 *	@return				The resulting ScriptObject
 */
template <class ERROR_HANDLER>
inline ScriptObject ScriptObject::callFunction( const ScriptArgs & args, 
		const ScriptDict & kwargs, const ERROR_HANDLER & errorHandler ) const
{
	PyObject * retval = NULL;
	if (!PyCallable_Check( this->get() ))
	{
		PyErr_Format( PyExc_TypeError, "Function is not callable" );
	}
	else
	{
		const ScriptArgs & safeArgs = args ? args : ScriptArgs::none();
		retval = PyObject_Call( this->get(), safeArgs.get(), kwargs.get() );
	}
	
	errorHandler.checkPtrError( retval );

	return ScriptObject( retval, ScriptObject::FROM_NEW_REFERENCE );
}


/**
 *	This method calls the ScriptObject function
 *
 *	@param args			The arguments to call the method with
 *	@param errorHandler The type of error handling to use if this method
 *		fails
 *	@return				The resulting ScriptObject
 */
template <class ERROR_HANDLER>
inline ScriptObject ScriptObject::callFunction( const ScriptArgs & args,
	const ERROR_HANDLER & errorHandler ) const
{
	return this->callFunction( args, ScriptDict(), errorHandler );
}


/**
 *	This method calls the ScriptObject function
 *
 *	@param errorHandler The type of error handling to use if this method
 *		fails
 *	@return				The resulting ScriptObject
 */
template <class ERROR_HANDLER>
inline ScriptObject ScriptObject::callFunction(
		const ERROR_HANDLER & errorHandler ) const
{
	return this->callFunction( ScriptArgs::none(), errorHandler );
}


/**
 *	This method gets the iterator for the object
 *
 *	@param errorHandler The type of error handling to use if this method
 *		fails
 *	@return				The resulting ScriptObject
 */
template <class ERROR_HANDLER>
inline ScriptIter ScriptObject::getIter( 
	const ERROR_HANDLER & errorHandler ) const
{
	PyObject * pIter = PyObject_GetIter( this->get() );
	errorHandler.checkPtrError( pIter );
	return ScriptIter( pIter, ScriptObject::FROM_NEW_REFERENCE );
}


/**
 *	Equivalent to the Python expression dir(o),
 *
 *	@param errorHandler The type of error handling to use if this method
 *		fails
 *	@return A (possibly empty) list of strings appropriate for the object
 *		argument, or NULL if there was an error. 
 */
template <class ERROR_HANDLER>
inline ScriptObject ScriptObject::getDir(
	const ERROR_HANDLER & errorHandler ) const
{
	if (this->get() == NULL)
	{
		// Do not call PyObject_Dir( NULL ) because it will get the names of
		// current locals not attached to this ScriptObject
		return ScriptObject( NULL );
	}

	PyObject* result = PyObject_Dir( this->get() );
	errorHandler.checkPtrError( result );
	return ScriptObject( result, ScriptObject::FROM_NEW_REFERENCE );
}


/**
 *	This method gets string for the object
 *
 *	@param errorHandler The type of error handling to use if this method
 *		fails
 *	@return				The resulting ScriptObject
 */
template <class ERROR_HANDLER>
inline ScriptString ScriptObject::str( 
	const ERROR_HANDLER & errorHandler ) const
{
	PyObject * pStr = PyObject_Str( this->get() );
	errorHandler.checkPtrError( pStr );
	return ScriptString( pStr, ScriptObject::FROM_NEW_REFERENCE );
}


/**
 *	Equivalent to Python built-in id( object ).
 *	@see builtinmodule.c builtin_id().
 *
 *	@param errorHandler The type of error handling to use if this method.
 *		fails
 *	@return				The id as a ScriptLong.
 */
inline ScriptLong ScriptObject::id() const
{
	// PyLong_FromVoidPtr may return an integer or long integer
	PyObject * pLongOrInt = PyLong_FromVoidPtr( this->get() );

	if (PyLong_CheckExact( pLongOrInt ) == 1)
	{
		return ScriptLong( pLongOrInt, ScriptObject::FROM_NEW_REFERENCE );
	}
	assert( PyInt_CheckExact( pLongOrInt ) == 1 );
	const ScriptInt intResult = ScriptInt( pLongOrInt, ScriptObject::FROM_NEW_REFERENCE );
	const long intToLong = intResult.asLong();
	return ScriptLong::create( intToLong );
}


/**
 *	This method creates a new ScriptObject from a given value
 *	@return A new script object
 */
template <class TYPE>
/* static */ inline ScriptObject ScriptObject::createFrom( TYPE val )
{
	return ScriptObject( Script::getData( val ),
			ScriptObject::FROM_NEW_REFERENCE );
}


/**
 *	This method converts a ScriptObject to a given type
 *	@param	rVal	The value to populate with the script object
 *	@return True on success, false otherwise
 */
template <class ERROR_HANDLER, class TYPE>
inline bool ScriptObject::convertTo( TYPE & rVal, 
	const ERROR_HANDLER & errorHandler ) const
{
	return convertTo( rVal, "", errorHandler );
}

/**
 *	This method converts a ScriptObject to a given type
 *	@param	rVal	The value to populate with the script object
 *	@param	varName	The name of the script object (to be stored as an error)
 *	@return True on success, false otherwise
 */
template <class ERROR_HANDLER, class TYPE>
inline bool ScriptObject::convertTo( TYPE & rVal, const char * varName,
	const ERROR_HANDLER & errorHandler ) const
{
	int ret = Script::setData( this->get(), rVal, varName );
	errorHandler.checkMinusOne( ret );
	return ret == 0;
}
