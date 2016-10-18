#ifndef GENERIC_COMPONENT_PROVIDER_HPP
#define GENERIC_COMPONENT_PROVIDER_HPP

#include "simple_component_provider.hpp"

namespace wgt
{
template< typename T >
class GenericComponentProvider : public SimpleComponentProvider
{
public:
	GenericComponentProvider( const char * component )
		: SimpleComponentProvider( component )
	{
	}

	GenericComponentProvider( const char * component,
		const ItemRole::Id roles[], size_t count )
		: SimpleComponentProvider( component, roles, count )
	{
	}

	const char * componentId( const TypeId & typeId,
		std::function< bool ( const ItemRole::Id& ) > & predicate ) const override
	{
		if (typeId != TypeId::getType<T>())
		{
			return nullptr;
		}

		return SimpleComponentProvider::componentId( typeId, predicate );
	}
};
} // end namespace wgt
#endif
