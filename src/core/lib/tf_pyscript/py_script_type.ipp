// py_script_type.ipp

/**
 *	This method gets the dict from a type.
 *
 *	@returns		The dict for the given type.
 */
inline ScriptDict ScriptType::getDict() const
{
	return ScriptDict( reinterpret_cast< PyTypeObject * >( this->get() )->tp_dict,
		ScriptObject::FROM_BORROWED_REFERENCE );
}
