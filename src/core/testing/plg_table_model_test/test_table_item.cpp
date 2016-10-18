#include "test_table_item.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/common_data_roles.hpp"

#include <vector>

namespace wgt
{

TestTableItem::TestTableItem( const char* value )
	: value_( value )
{
}


TestTableItem::~TestTableItem()
{
}


Variant TestTableItem::getData( ItemRole::Id roleId ) const
{
	if (roleId == ItemRole::displayId ||
		roleId == ItemRole::valueId)
	{
		return value_;
	}

	return Variant();
}


bool TestTableItem::setData( ItemRole::Id roleId, const Variant & data ) /* override */
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
