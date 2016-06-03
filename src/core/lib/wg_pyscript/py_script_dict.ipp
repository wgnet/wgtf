// py_script_dict.ipp

/**
 *	This method creates a new ScriptDict of a specified size
 *	@param capacity		The initial capacity of the dict
 */
/* static */ inline ScriptDict ScriptDict::create( size_type  capacity )
{
	PyObject * pDict = PyDict_New();
	assert( pDict );
	return ScriptDict( pDict, ScriptObject::FROM_NEW_REFERENCE );
}

/**
 *	This method sets an item within the dict
 *	@param key		The key to set the item of
 *	@param value	The value to set the item to
 *	@param errorHandler The type of error handling to use if this method
 *		fails
 *	@return			True if the value was successfully set, false otherwise
 */
template <class ERROR_HANDLER>
inline bool ScriptDict::setItem( const char * key, 
	const ScriptObject & value, const ERROR_HANDLER & errorHandler ) const
{
	int result = PyDict_SetItemString( this->get(),
			const_cast< char * >( key ), value.get() );
	errorHandler.checkMinusOne( result );
	return result == 0;
}

/**
 *	This method sets an item within the dict
 *	@param key		The key to set the item of
 *	@param value	The value to set the item to
 *	@param errorHandler The type of error handling to use if this method
 *		fails
 *	@return			True if the value was successfully set, false otherwise
 */
template <class ERROR_HANDLER>
inline bool ScriptDict::setItem( const ScriptObject & key, 
	const ScriptObject & value, const ERROR_HANDLER & errorHandler ) const
{
	int result = PyDict_SetItem( this->get(), key.get(), value.get() );
	errorHandler.checkMinusOne( result );
	return result == 0;
}

/*
 *	This method gets an item from the dict
 *	@param key		The key of the the item to get
 *	@param errorHandler The type of error handling to use if this method
 *		fails
 *	@return			The value relating to key
 */
template <class ERROR_HANDLER>
inline ScriptObject ScriptDict::getItem( const char * key, 
	const ERROR_HANDLER & errorHandler ) const
{
	// Note: PyDict_GetItem does not set an error if it fails, however
	// PyString_FromString does set an error, so we break the two apart
	// rather then using PyDict_GetItemString as we do not know what caused
	// the error.
	PyObject * pKey = PyString_FromString( key );

	if (pKey == NULL)
	{
		errorHandler.handleError();
		return ScriptObject();
	}

	PyObject * pResult = PyDict_GetItem( this->get(), pKey );
	Py_DECREF( pKey );
	errorHandler.checkPtrNoException( pResult );
	return ScriptObject( pResult, ScriptObject::FROM_BORROWED_REFERENCE );
}

/*
 *	This method gets an item from the dict
 *	@param key		The key of the the item to get
 *	@param errorHandler The type of error handling to use if this method
 *		fails
 *	@return			The value relating to key
 */
template <class ERROR_HANDLER>
inline ScriptObject ScriptDict::getItem( const ScriptObject & key, 
	const ERROR_HANDLER & errorHandler ) const
{
	PyObject * pResult = PyDict_GetItem( this->get(), key.get() );
	errorHandler.checkPtrNoException( pResult );
	return ScriptObject( pResult, ScriptObject::FROM_BORROWED_REFERENCE );
}


/**
 *	Return a PyListObject containing all the keys from the dictionary,
 *	as in the dictionary method dict.keys().
 *	
 *	@param errorHandler The type of error handling to use if this method
 *		fails
 *	@return list of dict.keys().
 */
template <class ERROR_HANDLER>
inline ScriptList ScriptDict::keys( const ERROR_HANDLER & errorHandler ) const
{
	PyObject * pResult = PyDict_Keys( this->get() );
	errorHandler.checkPtrNoException( pResult );
	return ScriptList( pResult, ScriptObject::FROM_NEW_REFERENCE );
}


/*
 *	This method deletes an item from the dict
 *	@param key		The key of the the item to delete
 *	@param errorHandler The type of error handling to use if this method fails
 *	@return true if the item was successfully removed.
 */
template <class ERROR_HANDLER>
inline bool ScriptDict::delItem( const char * key, 
	const ERROR_HANDLER & errorHandler )
{
	int res = PyDict_DelItemString(this->get(), const_cast<char*>( key ) );
	errorHandler.checkMinusOne( res );
	return res == 0;
}


/*
 *	This method deletes an item from the dict
 *	@param key		The key of the the item to delete
 *	@param errorHandler The type of error handling to use if this method fails
 *	@return true if the item was successfully removed.
 */
template <class ERROR_HANDLER>
inline bool ScriptDict::delItem( const ScriptObject & key,
	const ERROR_HANDLER & errorHandler )
{
	int res = PyDict_DelItem( this->get(), key.get() );
	errorHandler.checkMinusOne( res );
	return res == 0;
}


/*
 *	This method gets the size of the dict
 *	@return			The size of the dict
 */
inline ScriptDict::size_type ScriptDict::size() const
{
	// TODO: Handle size() failure, only occurs if not valid type
	return PyDict_Size( this->get() );
}


/*
 *	This method updates this dict's values with the values of other
 *	@param other		The dict to get the updated values from
 *	@param errorHandler The type of error handling to use if this method
 *		fails
 *	@return				True on success, false otherwise
 */
template <class ERROR_HANDLER>
inline bool ScriptDict::update( const ScriptDict & other,
	const ERROR_HANDLER & errorHandler ) const
{
	int result = PyDict_Update( this->get(),
		const_cast<PyObject*>(other.get()) );
	errorHandler.checkMinusOne( result );
	return result != -1;
}


/*
 *	This method empties this dictionary of all key-value pairs
 */
inline void ScriptDict::clear() const
{
	PyDict_Clear( this->get() );
}
