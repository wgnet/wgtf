#ifndef I_BASE_REFLECTED_PROPERTY_HPP
#define I_BASE_REFLECTED_PROPERTY_HPP

#include "i_method.hpp"
#include <cassert>
#include <memory>
#include "core_variant/variant.hpp"

//TODO:
//add #include "i_class_member.hpp"
//inherit from IClassMember and make IMethod inherit from it as well.

namespace wgt
{
class TypeId;
class ObjectHandle;
class MetaBase;
class Variant;
class IDefinitionManager;

template<typename T> class ObjectHandleT;
class MetaBase;
typedef ObjectHandleT< MetaBase > MetaHandle;

class IBaseProperty;
typedef std::shared_ptr< IBaseProperty > IBasePropertyPtr;

/**
 *	Interface for storing info about a member/method of a class.
 *	A collection of all members of a class is stored in an IClassDefinition.
 */
class IBaseProperty: public IMethod
{
public:
	virtual ~IBaseProperty() {}

	/**
	 *	Get the type identifier of the property.
	 */
	virtual const TypeId & getType() const = 0;

	/**
	 *	Get the name of the property.
	 */
	virtual const char * getName() const = 0;

	/**
	 *	Get the hash of the property.
	 */
	virtual uint64_t getNameHash() const = 0;

	/**
	 *	Get metadata about the property.
	 *	Such as display or usage hints.
	 */
	virtual MetaHandle getMetaData() const = 0;
	virtual bool readOnly() const = 0;

	//TODO: remove isMethod and add separate accessors to the class definition for properties and methods.
	virtual bool isMethod() const = 0;

	/**
	 *	Check if the property has a value that can be got.
	 *	e.g. if the property is a method then it may not have a value.
	 *	e.g. if the property is a function object then it may be both a method
	 *		and a value.
	 */
	virtual bool isValue() const = 0;


	/**
	 *	Set the value on the given property.
	 *	
	 *	@param handle the object which contains the value.
	 *	@param value the desired value to be set on the handle.
	 *	@param definitionManager manager which should contain the
	 *		IClassDefinition for the given handle and this property.
	 *	
	 *	@pre the handle must have this property in its IClassDefinition.
	 *	@pre the property must not be a method.
	 *	@pre the IClassDefinition for the given handle must be contained in
	 *		the given definitionManager.
	 *	@pre readOnly() must return false.
	 *	
	 *	@return true if the property was successfully set.
	 */
	virtual bool set( const ObjectHandle & handle,
		const Variant & value,
		const IDefinitionManager & definitionManager ) const = 0;


	/**
	 *	Get the value from the given property.
	 *	
	 *	@param handle the object which contains the value.
	 *	@param definitionManager manager which should contain the
	 *		IClassDefinition for the given handle and this property.
	 *	
	 *	@pre the handle must have this property in its IClassDefinition.
	 *	@pre the property must not be a method.
	 *	@pre the IClassDefinition for the given handle must be contained in
	 *		the given definitionManager.
	 *	@pre isValue() must return true.
	 *	
	 *	@return a Variant containing the value of the property on success.
	 *		A Variant containing 0 on failure.
	 */
	virtual Variant get( const ObjectHandle & handle,
		const IDefinitionManager & definitionManager ) const = 0;


	/**
	 *	If this property is a method, call it as a function.
	 *	
	 *	@param object the object which contains the method.
	 *	@param parameters arguments to the method.
	 *	
	 *	@pre the object must have this property in its IClassDefinition.
	 *	@pre the property must be a method, not a member variable.
	 *	@pre isMethod() must return true.
	 *	
	 *	@return a Variant containing the result of the function call on success.
	 *		A Variant containing 0 on failure.
	 */
	virtual Variant invoke( const ObjectHandle & object,
		const IDefinitionManager & definitionManager,
		const ReflectedMethodParameters & parameters ) = 0;


	/**
	 *	If this property is a method, get the number of arguments it takes.
	 *	
	 *	@pre the property must be a method, not a member variable.
	 *	
	 *	@return the number of arguments or 0 on failure.
	 */
	virtual size_t parameterCount() const = 0;
};
} // end namespace wgt
#endif // I_BASE_REFLECTED_PROPERTY_HPP
