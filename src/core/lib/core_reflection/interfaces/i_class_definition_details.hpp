#ifndef I_CLASS_DEFINITION_DETAILS_HPP
#define I_CLASS_DEFINITION_DETAILS_HPP

#include "core_reflection/interfaces/i_base_property.hpp"

#include <unordered_map>
#include <memory>

namespace wgt
{
class ObjectHandle;

class IClassDefinition;
class IClassDefinitionModifier;
class PropertyIteratorImplBase;
typedef std::shared_ptr< PropertyIteratorImplBase > PropertyIteratorImplPtr;

class TypeId;

template<typename T> class ObjectHandleT;
class MetaBase;
typedef ObjectHandleT< MetaBase > MetaHandle;

/**
 *	Interface for providing inheritance info about a type.
 *	Add implementations for extracting info from types in different languages.
 *	E.g. C++ classes, QML objects, Python classes.
 */
class IClassDefinitionDetails
{
public:
	virtual ~IClassDefinitionDetails() {}

	/**
	 *	Check if this type is an interface or a concrete type.
	 *	@return true if the type is an interface.
	 */
	virtual bool isAbstract() const = 0;

	/**
	 *	Check if this type is a generic definition.
	 *	
	 *	A definition is generic if it is defined at runtime and does not
	 *	persist external to the application lifetime,
	 *	unlike a definition that has been built from a static type.
	 *	
	 *	The serializer needs this information to determine how the
	 *	definition is to be serialized.
	 *	
	 *	@return true if the type is generated at runtime.
	 */
	virtual bool isGeneric() const = 0;

	/**
	 *	Get the name of the type that this defines.
	 *	@return the name of the type. Should not be an empty string or null.
	 */
	virtual const char * getName() const = 0;

	/**
	 *	Get the name of the parent/base class.
	 *	@note does not support multiple inheritance.
	 *	@return the name of the parent/base class or null if there isn't one.
	 */
	virtual const char * getParentName() const = 0;
	virtual MetaHandle getMetaData() const = 0;
	virtual ObjectHandle create(
		const IClassDefinition & classDefinition ) const = 0;
	virtual void * upCast( void * object ) const = 0;

	/**
	 *	Check if this implementation can lookup a property by name, if possible.
	 *	This only works if the IClassDefinitionDetails' implementation allows
	 *	lookup.
	 *	Otherwise properties must be searched using getPropertyIterator().
	 *	@return true if directLookupProperty() works, false if not.
	 */
	virtual bool canDirectLookupProperty() const
	{
		return false;
	}


	/**
	 *	Lookup a property by name, if possible.
	 *	This only works if the IClassDefinitionDetails' implementation allows
	 *	lookup.
	 *	Otherwise properties must be searched using getPropertyIterator().
	 *	@param name name of the property to lookup.
	 *	@return the found property or nullptr.
	 */
	virtual IBasePropertyPtr directLookupProperty( const char * /*name*/ ) const
	{
		return nullptr;
	}


	virtual PropertyIteratorImplPtr getPropertyIterator() const = 0;

	virtual IClassDefinitionModifier * getDefinitionModifier() const = 0;
};

typedef std::unique_ptr<IClassDefinitionDetails> IClassDefintionDetailsPtr;

} // end namespace wgt
#endif // I_CLASS_DEFINITION_DETAILS_HPP
