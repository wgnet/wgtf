#ifndef TEST_TABLE_ITEM_HPP
#define TEST_TABLE_ITEM_HPP

#include "test_table_model.hpp"
#include "core_data_model/abstract_item.hpp"

#include <memory>

namespace wgt
{
class TestTableItem: public AbstractTableItem
{
public:
	TestTableItem( const char* value );
	virtual ~TestTableItem();

	virtual Variant getData( ItemRole::Id roleId ) const override;
	virtual bool setData( ItemRole::Id roleId, const Variant & data ) override;

	virtual Connection connectPreDataChanged( DataCallback callback ) override;
	virtual Connection connectPostDataChanged( DataCallback callback ) override;

private:
	std::string value_;

	Signal< DataSignature > preDataChanged_;
	Signal< DataSignature > postDataChanged_;
};
} // end namespace wgt
#endif
