#ifndef TEST_TREE_MODEL_HPP
#define TEST_TREE_MODEL_HPP

#include "core_data_model/i_tree_model.hpp"
#include "core_data_model/abstract_item_model.hpp"

#include <memory>

namespace wgt
{
class TestTreeModelOld: public ITreeModel
{
public:
	TestTreeModelOld();
	TestTreeModelOld( const TestTreeModelOld& rhs );
	virtual ~TestTreeModelOld();

	TestTreeModelOld& operator=( const TestTreeModelOld& rhs );

	virtual IItem* item( size_t index, const IItem* parent ) const override;
	virtual ItemIndex index( const IItem* item ) const override;
	virtual bool empty( const IItem* parent ) const override;
	virtual size_t size( const IItem* parent ) const override;
	virtual int columnCount() const override;
	virtual Variant getData( int column, size_t roleId ) const override;
	virtual bool setData( int column, size_t roleId, const Variant & data ) override;

	void test();

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

class TestTreeModel: public AbstractTreeModel
{
public:
	TestTreeModel();
	TestTreeModel( const TestTreeModel& rhs );
	virtual ~TestTreeModel();

	TestTreeModel& operator=( const TestTreeModel& rhs );

	virtual AbstractItem * item( const ItemIndex & index ) const override;
	virtual ItemIndex index( const AbstractItem * item ) const override;
    virtual Variant getData( int row, int column, size_t roleId ) const override;
    virtual bool setData( int row, int column, size_t roleId, const Variant & data ) override;

	virtual int rowCount( const AbstractItem * item ) const override;
	virtual int columnCount() const override;

	void test();

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif
