#include "test_table_item.hpp"
#include "core_data_model/i_item_role.hpp"

#include <vector>

namespace wgt
{
ITEMROLE( display )
ITEMROLE( value )


TestTableItem::TestTableItem( const char* value )
	: value_( value )
{
}


TestTableItem::~TestTableItem()
{
}


Variant TestTableItem::getData( size_t roleId ) const
{
	if (roleId == ItemRole::displayId ||
		roleId == ItemRole::valueId)
	{
		return value_;
	}

	return Variant();
}


bool TestTableItem::setData( size_t roleId, const Variant & data ) /* override */
{
	if (roleId == ItemRole::displayId ||
		roleId == ItemRole::valueId)
	{
		std::string value;
		if (data.tryCast( value ))
		{
			preDataChanged_( roleId, data );
			value_ = value;
			postDataChanged_( roleId, data );
			return true;
		}
	}

	return false;
}


Connection TestTableItem::connectPreDataChanged( DataCallback callback ) /* override */
{
	return preDataChanged_.connect( callback );
}


Connection TestTableItem::connectPostDataChanged( DataCallback callback ) /* override */
{
	return postDataChanged_.connect( callback );
}


} // end namespace wgt
