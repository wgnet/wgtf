#ifndef I_CLASS_DEFINITION_DETAILS_HPP
#define I_CLASS_DEFINITION_DETAILS_HPP

#include "core_reflection/property_iterator.hpp"
#include "core_reflection/ref_object_id.hpp"
#include "core_reflection/interfaces/i_property_path.hpp"
#include "core_object/i_managed_object.hpp"
#include "core_common/deprecated.hpp"
#include "core_variant/variant.hpp"
#include <unordered_map>
#include <memory>

namespace wgt
{
template <typename T> class ObjectHandleT;
class TypeId;
class ObjectHandle;

class IClassDefinition;
class IClassDefinitionModifier;
class MetaData;

typedef std::shared_ptr<class IObjectHandleStorage> ObjectHandleStoragePtr;
typedef std::unique_ptr<PropertyIteratorImplBase> PropertyIteratorImplPtr;


/**
 *	Interface for providing inheritance info about a type.
 *	Add implementations for extracting info from types in different languages.
 *	E.g. C++ classes, QML objects, Python classes.
 */
class IClassDefinitionDetails
{
public:
	virtual ~IClassDefinitionDetails()
	{
	}


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
	virtual const char* getName() const = 0;

	virtual const MetaData & getMetaData() const = 0;
	virtual ObjectHandleStoragePtr createObjectStorage(const IClassDefinition& classDefinition) const = 0;
	virtual ManagedObjectPtr createManaged(const IClassDefinition& classDefinition, RefObjectId id = RefObjectId::zero()) const = 0;


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
	virtual IBasePropertyPtr directLookupProperty(const char* /*name*/) const
	{
		return nullptr;
	}

	//--------------------------------------------------------------------------
	virtual IBasePropertyPtr directLookupProperty( IPropertyPath::ConstPtr & path ) const
	{
		return directLookupProperty( path->getPath().c_str() );
	}

	virtual PropertyIteratorImplPtr getPropertyIterator() const = 0;

	virtual IClassDefinitionModifier* getDefinitionModifier() const = 0;

    //-----------------------------------------------------------------------------

    template<typename T = void>
    DEPRECATED ObjectHandle create(const IClassDefinition& classDefinition) const
    {
        static_assert(!std::is_same<T, T>::value,
            "This method is now deprecated. Please use ManagedObject");
    }
};

typedef std::unique_ptr<IClassDefinitionDetails> IClassDefintionDetailsPtr;

} // end namespace wgt
#endif // I_CLASS_DEFINITION_DETAILS_HPP
