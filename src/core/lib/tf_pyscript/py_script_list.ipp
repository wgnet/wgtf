// py_script_list.ipp

/**
 *	This method creates a new list, of size len
 *	@return A new list of size len
 */
/* static */ inline ScriptList ScriptList::create( Py_ssize_t len )
{
	PyObject * pList = PyList_New( len );
	assert( pList );
	return ScriptList( pList, ScriptObject::FROM_NEW_REFERENCE );
}


/**
 *	This method appends an object onto the end of the list
 *	@param object	The object to append to the list
 */
inline bool ScriptList::append( const ScriptObject & object ) const
{
	int result = PyList_Append( this->get(), object.get() );
	assert( result != -1 );
	return result == 0;
}


/**
 *	This method inserts an object into the list in front of index.
 *	@param pos		Item is inserted in front of this index.
 *	@param object	The object to append to the list
 *	@return true on successful insertion.
 */
inline bool ScriptList::insert( ScriptList::size_type pos,
	const ScriptObject & object ) const
{
	int result = PyList_Insert( this->get(), pos, object.get() );
	assert( result != -1 );
	return result == 0;
}


/**
 *	This method gets an item at a position
 *	@param pos	The position to get the item from.
 *		In the range 0 to +end.
 *			E.g. list[0] # start
 *				 list[len(list)-1] # end
 *		Or in the range -1 to -end.
 *			E.g. list[-1] # alias for end
 *				 list[-len(list)] # alias for start
 *	@return		The item at position pos
 */
inline ScriptObject ScriptList::getItem( ScriptList::size_type pos ) const
{
	assert( pos < PyList_GET_SIZE( this->get() ) );
	assert( -pos <= PyList_GET_SIZE( this->get() ) );
	PyObject * pItem = PyList_GET_ITEM( this->get(), pos );
	return ScriptObject( pItem, ScriptObject::FROM_BORROWED_REFERENCE );
}


/**
 *	This method sets an item at a position
 *	@param pos		The position to set the item att
 *		In the range 0 to +end.
 *			E.g. list[0] # start
 *				 list[len(list)-1] # end
 *		Or in the range -1 to -end.
 *			E.g. list[-1] # alias for end
 *				 list[-len(list)] # alias for start
 *	@param item		The item to set a position pos
 *	@return			This is always true
 */
inline bool ScriptList::setItem( ScriptList::size_type pos, ScriptObject item ) const
{
	assert( pos < PyList_GET_SIZE( this->get() ) );
	assert( -pos <= PyList_GET_SIZE( this->get() ) );
	// This steals a reference to the item, so must create a new ref for it
	PyList_SET_ITEM( this->get(), pos, item.newRef() );

	return true;
}


/**
 *	This method gets the size of the list
 *	@return		The size of the list
 */
inline ScriptList::size_type ScriptList::size() const
{
	return PyList_GET_SIZE( this->get() );
}


/**
 *	This method returns a new ScriptList which is a slice of this
 *	ScriptList including indexes [begin, end), as per the Python
 *	o[begin:end] syntax.
 *	@param begin The index of the first element of the slice
 *	@param end The index of the first element after the slice
 *	@note negative indexes are treated as 0
 */
template <class ERROR_HANDLER>
ScriptList ScriptList::getSlice( Py_ssize_t begin, Py_ssize_t end,
	const ERROR_HANDLER & errorHandler ) const
{
	PyObject * pResult = PyList_GetSlice( this->get(), begin, end );
	errorHandler.checkPtrError( pResult );
	return ScriptList( pResult, ScriptList::FROM_NEW_REFERENCE );
}

/**
 *	Set the slice of list between low and high to the contents of itemList.
 *	Analogous to list[low:high] = itemlist.
 *	@param begin start of slice.
 *		Negative indices, as when slicing from Python, are not supported.
 *	@param end end of slice.
 *		Negative indices, as when slicing from Python, are not supported.
 *	@param itemList list of items to set.
 *		may be NULL, indicating the assignment of an empty list (slice deletion).
 *	@return true on success.
 */
template <class ERROR_HANDLER>
bool ScriptList::setSlice( Py_ssize_t begin,
	Py_ssize_t end,
	const ScriptList & itemList,
	const ERROR_HANDLER & errorHandler )
{
	assert( (begin >= 0) && (begin < this->size()) );
	assert( (end > 0) && (end <= this->size()) );
	assert( begin < end );
	const int result = PyList_SetSlice( this->get(),
		begin,
		end,
		itemList.get() );
	errorHandler.checkMinusOne( result );
	return result == 0;
}
