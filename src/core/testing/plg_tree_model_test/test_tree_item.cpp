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

	AbstractTreeItem& main_;
	std::string name_;
	const AbstractTreeItem* parent_;

	Signal< DataSignature > preDataChanged_;
	Signal< DataSignature > postDataChanged_;
};

TestTreeItem::Implementation::Implementation(
	TestTreeItem& main, const char* name, const AbstractTreeItem* parent )
	: main_( main )
	, name_( name )
	, parent_( parent )
{
}


TestTreeItem::TestTreeItem( const char* name, const AbstractTreeItem* parent )
	: impl_( new Implementation( *this, name, parent ) )
{
}


TestTreeItem::~TestTreeItem()
{
}


const AbstractTreeItem* TestTreeItem::getParent() const
{
	return impl_->parent_;
}

Variant TestTreeItem::getData( int column, size_t roleId ) const
{
	if (column == 0)
	{
		if (roleId == ItemRole::displayId ||
			roleId == ItemRole::valueId)
		{
			return impl_->name_;
		}
	}
	else
	{
		if (roleId == ItemRole::displayId ||
			roleId == ItemRole::valueId)
		{
			return "Value";
		}
	}

	return Variant();
}


bool TestTreeItem::setData( int column, size_t roleId, const Variant& data ) /* override */
{
	if (column == 0)
	{
		if (roleId == ItemRole::displayId ||
			roleId == ItemRole::valueId)
		{
			std::string name;
			if (data.tryCast( name ))
			{
				impl_->preDataChanged_( column, roleId, data );
				impl_->name_ = name;
				impl_->postDataChanged_( column, roleId, data );
				return true;
			}
		}
	}

	return false;
}


Connection TestTreeItem::connectPreDataChanged( DataCallback callback ) /* override */
{
	return impl_->preDataChanged_.connect( callback );
}


Connection TestTreeItem::connectPostDataChanged( DataCallback callback ) /* override */
{
	return impl_->postDataChanged_.connect( callback );
}


} // end namespace wgt
