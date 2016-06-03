#ifndef I_META_TYPE_MANAGER_HPP
#define I_META_TYPE_MANAGER_HPP

#include <typeinfo>
#include "core_dependency_system/i_interface.hpp"
#include "core_variant/type_id.hpp"

namespace wgt
{
class MetaType;

class IMetaTypeManager
{
public:
	virtual ~IMetaTypeManager() {}

	/**
	Register user type.
	Use this function to add support of any type.

	MetaType ownership is not transferred to Variant, so you're responsible
	for its life time management. You must ensure that MetaType is valid
	while at least one instance of Variant with value of corresponding type
	exist. Undefined behavior (most likely crash) may occur otherwise.

	Returns whether type was successfully registered. Registration may fail when
	the type with the same name and/or matching @c TypeId is already
	registered.

	@see typeIsRegistered, findType
	*/
	virtual bool registerType(const MetaType* type) = 0;


	/**
	 *	Deregister user type.
	 *	
	 *	@pre type must previously been registered with registerType.
	 *	@pre all Variants using the type must have been destroyed.
	 *	
	 *	@param type to be deregistered. Must not be null.
	 *	@return true on success.
	 */
	virtual bool deregisterType(const MetaType* type) = 0;


	template< typename T >
	const MetaType* findType() const
	{
		return findType( typeid( T ) );
	}

	/**
	Find registered MetaType by name.

	@see registerType
	*/
	virtual const MetaType* findType(const char* name) const = 0;

	/**
	Find registered MetaType by @c TypeId.

	@see registerType
	*/
	virtual const MetaType* findType(const TypeId& typeId) const = 0;

	const MetaType* findType(const std::type_info& typeInfo) const
	{
		return findType( TypeId( typeInfo.name() ) );
	}

};
} // end namespace wgt
#endif // I_META_TYPE_MANAGER_HPP
