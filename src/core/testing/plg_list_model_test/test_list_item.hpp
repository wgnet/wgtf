#ifndef TEST_LIST_ITEM_HPP
#define TEST_LIST_ITEM_HPP

#include "core_data_model/i_item.hpp"
#include "test_list_model.hpp"
#include "core_data_model/abstract_item.hpp"

#include <memory>

namespace wgt
{
class OldTestListItem: public IItem
{
public:
	OldTestListItem( const char* name, const char* value );
	OldTestListItem( const char* name, const size_t value );
	OldTestListItem( const OldTestListItem& rhs );
	virtual ~OldTestListItem();

	OldTestListItem& operator=( const OldTestListItem& rhs );

	virtual const char* getDisplayText( int column ) const override;
	virtual ThumbnailData getThumbnail( int column ) const override;
	virtual Variant getData( int column, ItemRole::Id roleId ) const override;
	virtual bool setData( int column, ItemRole::Id roleId, const Variant& data ) override;

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};

class TestListItem: public AbstractListItem
{
public:
	TestListItem( const char* name, const char* value );
	TestListItem( const char* name, const size_t value );
	TestListItem( const TestListItem& rhs );
	virtual ~TestListItem();

	TestListItem& operator=( const TestListItem& rhs );

	virtual Variant getData( int column, ItemRole::Id roleId ) const override;
	virtual bool setData( int column, ItemRole::Id roleId, const Variant & data ) override;

	virtual Connection connectPreDataChanged( DataCallback callback ) override;
	virtual Connection connectPostDataChanged( DataCallback callback ) override;

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif
