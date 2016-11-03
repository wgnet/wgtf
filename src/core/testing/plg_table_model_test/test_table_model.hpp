#ifndef TEST_TABLE_MODEL_HPP
#define TEST_TABLE_MODEL_HPP

#include "core_data_model/abstract_item_model.hpp"
#include "testing/data_model_test/test_data.hpp"

#include <memory>

namespace wgt
{
class TestTableItem;
class TestTableModel: public AbstractTableModel
{
public:
	TestTableModel();
	virtual ~TestTableModel();

	virtual Variant getData( int row, int column, ItemRole::Id roleId ) const override;
	virtual bool setData( int row, int column, ItemRole::Id roleId, const Variant & data ) override;

	virtual AbstractItem * item( const ItemIndex & index ) const override;
	virtual ItemIndex index( const AbstractItem * item ) const override;

	virtual int rowCount() const override;
	virtual int columnCount() const override;

	virtual std::vector< std::string > roles() const override;

	virtual Connection connectPreItemDataChanged( DataCallback callback ) override;
	virtual Connection connectPostItemDataChanged( DataCallback callback ) override;

private:
	void generateData();
	void clear();

	std::vector<TestTableItem*> items_;
	StringList2 dataSource_;
	std::vector<std::string> headerText_;
	std::vector<std::string> footerText_;

	Signal< DataSignature > preDataChanged_;
	Signal< DataSignature > postDataChanged_;
};
} // end namespace wgt
#endif
