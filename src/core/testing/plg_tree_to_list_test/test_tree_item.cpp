#include "test_tree_item.hpp"
#include "core_data_model/i_item_role.hpp"

namespace wgt
{
struct TestTreeItem::Implementation
{
	Implementation( TestTreeItem& main, const char* name, const IItem* parent );
	~Implementation();

	TestTreeItem& main_;
	const char* name_;
	const IItem* parent_;
};

TestTreeItem::Implementation::Implementation(
	TestTreeItem& main, const char* name, const IItem* parent )
	: main_( main )
	, name_( name )
	, parent_( parent )
{
}

TestTreeItem::Implementation::~Implementation()
{
	delete name_;
}


TestTreeItem::TestTreeItem( const char* name, const IItem* parent )
	: impl_( new Implementation( *this, name, parent ) )
{
}

TestTreeItem::TestTreeItem( const TestTreeItem& rhs )
	: impl_( new Implementation( *this, rhs.impl_->name_, rhs.impl_->parent_ ) )
{
}

TestTreeItem::~TestTreeItem()
{
}

TestTreeItem& TestTreeItem::operator=( const TestTreeItem& rhs )
{
	if (this != &rhs)
	{
		impl_.reset( new Implementation(
			*this, rhs.impl_->name_, rhs.impl_->parent_ ) );
	}

	return *this;
}

const IItem* TestTreeItem::getParent() const
{
	return impl_->parent_;
}

const char* TestTreeItem::getDisplayText( int column ) const
{
	return impl_->name_;
}

ThumbnailData TestTreeItem::getThumbnail( int column ) const
{
	return nullptr;
}

Variant TestTreeItem::getData( int column, ItemRole::Id roleId ) const
{
	if (roleId == ValueRole::roleId_ && column == 0)
	{
		return impl_->name_;
	}
	else if (roleId == IndexPathRole::roleId_)
	{
		return impl_->name_;
	}

	return Variant();
}

bool TestTreeItem::setData( int column, ItemRole::Id roleId, const Variant& data )
{
	return false;
}
} // end namespace wgt
