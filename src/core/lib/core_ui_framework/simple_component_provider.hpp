#ifndef SIMPLE_COMPONENT_PROVIDER_HPP
#define SIMPLE_COMPONENT_PROVIDER_HPP

#include "i_component_provider.hpp"

#include <array>

namespace wgt
{
class SimpleComponentProvider : public IComponentProvider
{
public:
	SimpleComponentProvider( const char * component )
		: component_( component )
	{
	}

	SimpleComponentProvider( const char * component,
		const size_t roles[], size_t count )
		: component_( component )
	{
		for (size_t i = 0; i < count; ++i)
		{
			roles_.push_back( roles[i] );
		}
	}

	const char * componentId( const TypeId & typeId,
		std::function< bool ( size_t ) > & predicate ) const override
	{
		for ( auto role : roles_ )
		{
			if (!predicate( role ))
			{
				return nullptr;
			}
		}

		return component_.c_str();
	}

private:
	std::string component_;
	std::vector< size_t > roles_;
};
} // end namespace wgt
#endif
