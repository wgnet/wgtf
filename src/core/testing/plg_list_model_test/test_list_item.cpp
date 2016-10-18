#include "test_list_item.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/common_data_roles.hpp"

#include <vector>

namespace wgt
{

enum class TestListItemType
{
	STRING,
	NUMBER
};

struct OldTestListItem::Implementation
{
	Implementation(
		OldTestListItem& self,
		const std::string& name,
		const std::string& value );
	Implementation(
		OldTestListItem& self,
		const std::string& name,
		size_t value );
	~Implementation();

	OldTestListItem& self_;
	TestListItemType type_;
	std::string name_;
	std::string string_;
	size_t number_;
};


OldTestListItem::Implementation::Implementation(
	OldTestListItem& self,
	const std::string& name,
	const std::string& value )
	: self_( self )
	, type_( TestListItemType::STRING )
	, name_( name )
	, string_( value )
	, number_( 0 )
{
}


OldTestListItem::Implementation::Implementation(
	OldTestListItem& self,
	const std::string& name,
	size_t value )
	: self_( self )
	, type_( TestListItemType::NUMBER )
	, name_( name )
	, number_( value )
{
}


OldTestListItem::Implementation::~Implementation()
{
}


OldTestListItem::OldTestListItem( const char* name, const char* value )
	: impl_( new Implementation( *this, name, value ) )
{
}


OldTestListItem::OldTestListItem( const char* name, const size_t value )
	: impl_( new Implementation( *this, name, value ) )
{
}


OldTestListItem::OldTestListItem( const OldTestListItem& rhs )
{
	Implementation& from = *rhs.impl_.get();

	if (from.type_ == TestListItemType::STRING)
	{
		impl_.reset( new Implementation( *this, from.name_, from.string_ ) );
	}
	else
	{
		impl_.reset( new Implementation( *this, from.name_, from.number_ ) );
	}
}


OldTestListItem::~OldTestListItem()
{
}


OldTestListItem& OldTestListItem::operator=( const OldTestListItem& rhs )
{
	if (this != &rhs)
	{
		Implementation& from = *rhs.impl_.get();

		if (from.type_ == TestListItemType::STRING)
		{
			impl_.reset( new Implementation(
				*this, from.name_, from.string_ ) );
		}
		else
		{
			impl_.reset( new Implementation(
				*this, from.name_, from.number_ ) );
		}
	}

	return *this;
}


const char* OldTestListItem::getDisplayText( int column ) const
{
	if (column == 0)
	{
		return impl_->name_.c_str();
	}
	else if (column == 1)
	{
		return "Value";
	}

	return nullptr;
}


ThumbnailData OldTestListItem::getThumbnail( int column ) const
{
	return nullptr;
}


Variant OldTestListItem::getData( int column, ItemRole::Id roleId ) const
{
	if (roleId == ValueTypeRole::roleId_)
	{
		if (column == 0 || impl_->type_ == TestListItemType::STRING)
		{
			return typeid( std::string ).name();
		}
		else
		{
			return typeid( size_t ).name();
		}
	}
	else if (roleId == ValueRole::roleId_)
	{
		if (column == 0)
		{
			return impl_->name_;
		}
		else if (impl_->type_ == TestListItemType::STRING)
		{
			return impl_->string_;
		}
		else
		{
			return impl_->number_;
		}
	}
	else if (roleId == IndexPathRole::roleId_)
	{
		return impl_->name_;
	}

	return Variant();
}


bool OldTestListItem::setData( int column, ItemRole::Id roleId, const Variant& data )
{
	if (column == 1)
	{
		std::string string;
		size_t number;

		if (data.tryCast( string ))
		{
			impl_->string_ = string;
			impl_->number_ = 0;
			impl_->type_ = TestListItemType::STRING;
		}
		else if (data.tryCast( number ))
		{
			impl_->string_ = "";
			impl_->number_ = number;
			impl_->type_ = TestListItemType::NUMBER;
		}
	}

	return false;
}


struct TestListItem::Implementation
{
	Implementation(
		TestListItem& self,
		const std::string& name,
		const std::string& value );
	Implementation(
		TestListItem& self,
		const std::string& name,
		size_t value );
	~Implementation();

	TestListItem& self_;
	TestListItemType type_;
	std::string name_;
	std::string string_;
	size_t number_;
	Signal< TestListItem::DataSignature > preDataChanged_;
	Signal< TestListItem::DataSignature > postDataChanged_;
};


TestListItem::Implementation::Implementation(
	TestListItem& self,
	const std::string& name,
	const std::string& value )
	: self_( self )
	, type_( TestListItemType::STRING )
	, name_( name )
	, string_( value )
	, number_( 0 )
{
}


TestListItem::Implementation::Implementation(
	TestListItem& self,
	const std::string& name,
	size_t value )
	: self_( self )
	, type_( TestListItemType::NUMBER )
	, name_( name )
	, number_( value )
{
}


TestListItem::Implementation::~Implementation()
{
}


TestListItem::TestListItem( const char* name, const char* value )
	: impl_( new Implementation( *this, name, value ) )
{
}


TestListItem::TestListItem( const char* name, const size_t value )
	: impl_( new Implementation( *this, name, value ) )
{
}


TestListItem::TestListItem( const TestListItem& rhs )
{
	Implementation& from = *rhs.impl_.get();

	if (from.type_ == TestListItemType::STRING)
	{
		impl_.reset( new Implementation( *this, from.name_, from.string_ ) );
	}
	else
	{
		impl_.reset( new Implementation( *this, from.name_, from.number_ ) );
	}
}


TestListItem::~TestListItem()
{
}


Variant TestListItem::getData( int column, ItemRole::Id roleId ) const /* override */
{
	if (column == 0)
	{
		if (roleId == ItemRole::displayId)
		{
			return impl_->name_.c_str();
		}
		else
		{
			return "Value";
		}
	}
	else if (column == 1)
	{
		if (roleId == ItemRole::valueId)
		{
			if (impl_->type_ == TestListItemType::STRING)
			{
				return impl_->string_;
			}
			else
			{
				return impl_->number_;
			}
		}
	}

	return AbstractListItem::getData( column, roleId );
}


bool TestListItem::setData( int column, ItemRole::Id roleId, const Variant & data )
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
	else if (column == 1)
	{
		if (roleId == ItemRole::valueId)
		{
			std::string string;
			size_t number;

			if (data.tryCast( number ))
			{
				impl_->preDataChanged_( column, roleId, data );
				impl_->string_ = "";
				impl_->number_ = number;
				impl_->type_ = TestListItemType::NUMBER;
				impl_->postDataChanged_( column, roleId, data );
				return true;
			}
			else if (data.tryCast( string ))
			{
				impl_->preDataChanged_( column, roleId, data );
				impl_->string_ = string;
				impl_->number_ = 0;
				impl_->type_ = TestListItemType::STRING;
				impl_->postDataChanged_( column, roleId, data );
				return true;
			}
		}
	}

	return AbstractListItem::setData( column, roleId, data );
}


Connection TestListItem::connectPreDataChanged( DataCallback callback ) /* override */
{
	return impl_->preDataChanged_.connect( callback );
}


Connection TestListItem::connectPostDataChanged( DataCallback callback ) /* override */
{
	return impl_->postDataChanged_.connect( callback );
}


} // end namespace wgt
