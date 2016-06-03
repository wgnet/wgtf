#ifndef I_DEFINITION_HELPER_HPP
#define I_DEFINITION_HELPER_HPP

#include "core_variant/type_id.hpp"

namespace wgt
{
class IClassDefinition;
class ObjectHandle;

/**
 *	Interface for a helper class that allows external plugins to override
 *	core reflection system functionality for specific types
 */
class IDefinitionHelper
{
public:
	virtual ~IDefinitionHelper() {}

	/**
	 *	TypeId for which to override reflection functionality.
	 */
	virtual TypeId typeId() const = 0;

	/**
	 *	Get a definition from an ObjectHandle of the associated type
	 */
	virtual IClassDefinition * getDefinition( const ObjectHandle & object ) const = 0;
};
} // end namespace wgt
#endif
