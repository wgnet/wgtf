#ifndef TEST_TREE_ITEM_HPP
#define TEST_TREE_ITEM_HPP

#include "core_data_model/i_item.hpp"
#include "core_data_model/abstract_item.hpp"

#include <memory>

namespace wgt
{
class TestTreeItemOld: public IItem
{
public:
	TestTreeItemOld( const char* name, const IItem* parent );
	TestTreeItemOld( const TestTreeItemOld& rhs );
	virtual ~TestTreeItemOld();

	TestTreeItemOld& operator=( const TestTreeItemOld& rhs );

	const IItem* getParent() const;
	virtual const char* getDisplayText( int column ) const;
	virtual ThumbnailData getThumbnail( int column ) const;
	virtual Variant getData( int column, size_t roleId ) const;
	virtual bool setData( int column, size_t roleId, const Variant& data );

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

class TestTreeItem: public AbstractTreeItem
{
public:
	TestTreeItem( const char* name, const AbstractTreeItem* parent );
	virtual ~TestTreeItem();

	const AbstractTreeItem* getParent() const;

	virtual Variant getData( int column, size_t roleId ) const override;
	virtual bool setData( int column, size_t roleId, const Variant& data ) override;

	virtual Connection connectPreDataChanged( DataCallback callback ) override;
	virtual Connection connectPostDataChanged( DataCallback callback ) override;

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif
