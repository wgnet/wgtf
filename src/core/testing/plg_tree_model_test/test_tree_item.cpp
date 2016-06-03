#include "test_tree_item.hpp"
#include "core_data_model/i_item_role.hpp"

namespace wgt
{
ITEMROLE( display )
ITEMROLE( value )

struct TestTreeItemOld::Implementation
{
	Implementation( TestTreeItemOld& main, const char* name, const IItem* parent );
	~Implementation();

	TestTreeItemOld& main_;
	const char* name_;
	const IItem* parent_;
};

TestTreeItemOld::Implementation::Implementation(
	TestTreeItemOld& main, const char* name, const IItem* parent )
	: main_( main )
	, name_( name )
	, parent_( parent )
{
}

TestTreeItemOld::Implementation::~Implementation()
{
	delete name_;
}


TestTreeItemOld::TestTreeItemOld( const char* name, const IItem* parent )
	: impl_( new Implementation( *this, name, parent ) )
{
}

TestTreeItemOld::TestTreeItemOld( const TestTreeItemOld& rhs )
	: impl_( new Implementation( *this, rhs.impl_->name_, rhs.impl_->parent_ ) )
{
}

TestTreeItemOld::~TestTreeItemOld()
{
}

TestTreeItemOld& TestTreeItemOld::operator=( const TestTreeItemOld& rhs )
{
	if (this != &rhs)
	{
		impl_.reset( new Implementation(
			*this, rhs.impl_->name_, rhs.impl_->parent_ ) );
	}

	return *this;
}

const IItem* TestTreeItemOld::getParent() const
{
	return impl_->parent_;
}

const char* TestTreeItemOld::getDisplayText( int column ) const
{
	return impl_->name_;
}

ThumbnailData TestTreeItemOld::getThumbnail( int column ) const
{
	return nullptr;
}

Variant TestTreeItemOld::getData( int column, size_t roleId ) const
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

bool TestTreeItemOld::setData( int column, size_t roleId, const Variant& data )
{
	return false;
}

struct TestTreeItem::Implementation
{
	Implementation( TestTreeItem& main, const char* name, const AbstractTreeItem* parent );
	~Implementation();

	AbstractTreeItem& main_;
	const char* name_;
	const AbstractTreeItem* parent_;
};

TestTreeItem::Implementation::Implementation(
	TestTreeItem& main, const char* name, const AbstractTreeItem* parent )
	: main_( main )
	, name_( name )
	, parent_( parent )
{
}

TestTreeItem::Implementation::~Implementation()
{
	delete name_;
}


TestTreeItem::TestTreeItem( const char* name, const AbstractTreeItem* parent )
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

const AbstractTreeItem* TestTreeItem::getParent() const
{
	return impl_->parent_;
}

Variant TestTreeItem::getData( int column, size_t roleId ) const
{
	if (roleId == ItemRole::displayId ||
		roleId == ItemRole::valueId)
	{
		return column == 0 ? impl_->name_ : "Value";
	}

	return Variant();
}

bool TestTreeItem::setData( int column, size_t roleId, const Variant& data )
{
	return false;
}
} // end namespace wgt
