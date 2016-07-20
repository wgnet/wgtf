#ifndef TEST_GRID_MODEL_HPP
#define TEST_GRID_MODEL_HPP

#include "core_data_model/abstract_item_model.hpp"
#include "testing/data_model_test/test_data.hpp"

#include <memory>

namespace wgt
{
class TestGridItem;
class TestGridModel: public AbstractTableModel
{
public:
	TestGridModel();
	virtual ~TestGridModel();

	virtual Variant getData( int row, int column, size_t roleId ) const override;
	virtual bool setData( int row, int column, size_t roleId, const Variant & data ) override;

	virtual AbstractItem * item( const ItemIndex & index ) const override;
	virtual ItemIndex index( const AbstractItem * item ) const override;

	virtual int rowCount() const override;
	virtual int columnCount() const override;

private:
	void generateData();
	void clear();

	std::vector<TestGridItem*> items_;
	StringList2 dataSource_;
	std::vector<std::string> headerText_;
	std::vector<std::string> footerText_;
};
} // end namespace wgt
#endif
