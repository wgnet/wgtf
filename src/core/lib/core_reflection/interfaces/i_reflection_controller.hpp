#ifndef I_REFLECTION_CONTROLLER_HPP
#define I_REFLECTION_CONTROLLER_HPP

#include "core_variant/variant.hpp"

namespace wgt
{
class PropertyAccessor;
class ReflectedMethodParameters;

class CommandInstance;
template <typename T>
class ObjectHandleT;
typedef ObjectHandleT<CommandInstance> CommandInstancePtr;

/**
 *	Handles getting, setting and invoking data on reflected objects via the
 *	Command System.
 */
class IReflectionController
{
public:
	/**
	 *	Get a reflected value.
	 *	This will block until all commands have modified it.
	 *	@param pa the property for which to get the value.
	 *	@return the value of the property.
	 */
	virtual Variant getValue(const PropertyAccessor& pa) = 0;
	/**
	 *	Set a reflected value.
	 *	This may execute on a background thread later.
	 *	@param pa the property for which to set the value.
	 *	@param data the value of the property.
	 */
	virtual void setValue(const PropertyAccessor& pa, const Variant& data) = 0;
	/**
	 *	Invoke a reflected property.
	 *	This will block to get the return value.
	 *	@param pa the property for which to set the value.
	 *	@param parameters arguments to the function.
	 *	@return the result of the function.
	 */
	virtual Variant invoke(const PropertyAccessor& pa, const ReflectedMethodParameters& parameters) = 0;
	/**
	 *	Insert an element into a reflected property.
	 *  Only valid for reflected properties that are collections
	 *	This will block
	 *	@param pa the property for which to insert the element.
	 *	@param key the key of the element to insert.
	 *	@param value the value of the element to insert.
	 */
	virtual void insert(const PropertyAccessor& pa, const Variant& key, const Variant& value) = 0;
	/**
	 *	Erase an element from a reflected property.
	 *  Only valid for reflected properties that are collections
	 *	This will block
	 *	@param pa the property from which to erase the element.
	 *	@param key the key of the element to erase.
	 */
	virtual void erase(const PropertyAccessor& pa, const Variant& key) = 0;
};
} // end namespace wgt
#endif // I_REFLECTION_CONTROLLER_HPP
