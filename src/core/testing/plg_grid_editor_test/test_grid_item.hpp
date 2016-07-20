#ifndef TEST_GRID_ITEM_HPP
#define TEST_GRID_ITEM_HPP

#include "test_grid_model.hpp"
#include "core_data_model/abstract_item.hpp"

#include <memory>

namespace wgt
{
class TestGridItem: public AbstractTableItem
{
public:
	TestGridItem( const char* value );
	virtual ~TestGridItem();

	virtual Variant getData( size_t roleId ) const override;
	virtual bool setData( size_t roleId, const Variant & data ) override;

private:
	std::string value_;
};
} // end namespace wgt
#endif
