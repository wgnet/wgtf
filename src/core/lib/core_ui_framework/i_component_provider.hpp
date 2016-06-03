#ifndef I_COMPONENT_PROVIDER_HPP
#define I_COMPONENT_PROVIDER_HPP

#include "core_variant/type_id.hpp"

#include <vector>

namespace wgt
{
class IComponentProvider
{
public:
	virtual ~IComponentProvider() {}

	virtual const char * componentId( const TypeId & typeId,
		std::function< bool ( size_t ) > & predicate ) const = 0;
};
} // end namespace wgt
#endif//I_COMPONENT_PROVIDER_HPP
