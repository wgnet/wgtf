#include "test_grid_item.hpp"
#include "core_data_model/i_item_role.hpp"

#include <vector>

namespace wgt
{
ITEMROLE( display )
ITEMROLE( value )


TestGridItem::TestGridItem( const char* value )
	: value_( value )
{
}


TestGridItem::~TestGridItem()
{
}

bool TestGridItem::setData( size_t roleId, const Variant & data )
{
	if (roleId == ItemRole::displayId ||
		roleId == ItemRole::valueId)
	{
		std::string value;
		bool canCast = data.tryCast( value );
		if (canCast)
		{
			value_ = value;
		}

		return canCast;
	}

	return false;
}

Variant TestGridItem::getData( size_t roleId ) const
{
	if (roleId == ItemRole::displayId ||
			roleId == ItemRole::valueId)
	{
		return value_;
	}

	return Variant();
}
} // end namespace wgt
