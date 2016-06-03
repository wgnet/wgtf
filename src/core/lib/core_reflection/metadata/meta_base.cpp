#include "meta_base.hpp"
#include "meta_types.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/function_property.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"

namespace wgt
{
BEGIN_EXPOSE( MetaBase, MetaNone() )
END_EXPOSE()

//==============================================================================
MetaBase::MetaBase()
{
}


//==============================================================================
/*virtual */MetaBase::~MetaBase()
{
}


//==============================================================================
const MetaHandle & operator + ( const MetaHandle & left, const MetaHandle & right )
{
	if (left == nullptr)
	{
		return right;
	}

	// traverse to the end of the linked list
	auto next = left->next();
	auto last = left;
	while( next != nullptr )
	{
		last = next;
		next = next->next();
	};

	// hook into the end
	last->setNext( right );

	return left; 
}
} // end namespace wgt
