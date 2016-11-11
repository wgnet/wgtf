#ifndef I_CLASS_DEFINITION_HPP
#define I_CLASS_DEFINITION_HPP

#include "../property_iterator.hpp"
#include "../ref_object_id.hpp"
#include <functional>

namespace wgt
{
class ObjectHandle;
class IClassDefinitionDetails;
class IBaseProperty;
class IDefinitionManager;
class PropertyAccessor;
class TypeId;

template <typename T>
class ObjectHandleT;
class MetaBase;
typedef ObjectHandleT<MetaBase> MetaHandle;

typedef std::function<void*()> DataGetter;

/**
 *	Interface for storing info that "defines" a class.
 *
 *	Info such as the names and types of members on a class and metadata about
 *	their usage.
 *	E.g. so you could iterate over each variable in a class and
 *	print out the name and value, serialize it to disk, add it to the UI etc.
 *
 *	Stores info about inheritance and if a class can be cast to a base or
 *	derived class.
 */
class REFLECTION_DLL IClassDefinition
{
public:
	static const char INDEX_OPEN = '[';
	static const char INDEX_CLOSE = ']';
	static const char DOT_OPERATOR = '.';

	virtual ~IClassDefinition()
	{
	}

	/**
	 *	Get the info extracted from a class.
	 *	E.g. using REGISTER_DEFINITION( type ) will extract info about
	 *		inheritance.
	 */
	virtual const IClassDefinitionDetails& getDetails() const = 0;

	/**
	 *	Create a PropertyAccessor bound to a property on the given object.
	 *
	 *	@param name name of the property to bind.
	 *	@param object the object which contains the value.
	 *
	 *	@pre the object's IClassDefinition must match this IClassDefinition.
	 *
	 *	@return an accessor bound to the property on the object on success.
	 *		The accessor will not be valid on failure.
	 */
	virtual PropertyAccessor bindProperty(const char* name, const ObjectHandle& object) const = 0;

	// TODO: Remove, only used by ClassDefinition itself
	virtual void bindPropertyImpl(const char* name, const ObjectHandle& pBase,
	                              PropertyAccessor& o_PropertyAccessor) const = 0;

	/**
	 *	Get the manager in which this definition belongs.
	 */
	virtual IDefinitionManager* getDefinitionManager() const = 0;

	/**
	 *	Set the manager in which this definition belongs.
	 */
	virtual void setDefinitionManager(IDefinitionManager* defManager) = 0;

	/**
	 *	Check if this class definition can have properties added and removed
	 *	after construction.
	 *	@return true if it is possible to add/remove properties.
	 */
	virtual bool isGeneric() const = 0;

	/**
	 *	Get the definition for the base class from which this class inherits.
	 *	@return base class definition or null if there is no base class.
	 */
	virtual IClassDefinition* getParent() const = 0;

	/**
	 *	Check if this class definition can be cast up/down the inheritance
	 *	chain to another definition.
	 */
	virtual bool canBeCastTo(const IClassDefinition& definition) const = 0;

	/**
	 *	Cast the given object from this definition to another type of definition.
	 *
	 *	@param definition the definition to which to cast.
	 *	@param object the object to be cast to the new definition.
	 *
	 *	@pre the object's definition should match this definition.
	 *
	 *	@return the object cast to the other definition or null on failure.
	 */
	virtual void* castTo(const IClassDefinition& definition, void* object) const = 0;

	/**
	 *	Cast the given object of another definition to this definition,
	 *	if this definition is a parent/base class.
	 *
	 *	@param object the object to be cast to the base class definition.
	 *
	 *	@pre the object's class should inherit from this class.
	 *
	 *	@return the object cast to this definition or null on failure.
	 */
	virtual void* upCast(void* object) const = 0;

	/**
	 *	Range for all properties contained in this and its parents' definitions.
	 */
	virtual PropertyIteratorRange allProperties() const = 0;

	/**
	 *	Range for only properties contained in this definition.
	 */
	virtual PropertyIteratorRange directProperties() const = 0;

	/**
	 *	Find the first property with the given ID.
	 *	Searches all properties, direct and parent.
	 */
	virtual IBasePropertyPtr findProperty(const char* name, size_t length) const = 0;
	IBasePropertyPtr findProperty(const char* name) const
	{
		return findProperty(name, strlen(name));
	}

	/**
	 *	Get an identifier for this definition's type.
	 */
	virtual const char* getName() const = 0;

	/**
	 *	Get metadata about this definition.
	 */
	virtual MetaHandle getMetaData() const = 0;

	/**
	 *	Create an object based on this definition.
	 *	If this is a static definition (per type), then create an instance of a class.
	 *	If this is a dynamic definition (per instance), then clone the instance.
	 *	@return new object handle instance.
	 */
	virtual ObjectHandle create() const = 0;

	/**
	 *	Create an object based on this definition which is managed by
	 *	an IObjectManager.
	 *	i.e. like creating an instance of a class.
	 *	@return new object handle instance.
	 */
	virtual ObjectHandle createManagedObject(const RefObjectId& id = RefObjectId::zero()) const = 0;
};
} // end namespace wgt
#endif // I_CLASS_DEFINITION_HPP
