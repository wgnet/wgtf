#include "test_list_item.hpp"
#include "core_data_model/i_item_role.hpp"

#include <vector>

namespace wgt
{
ITEMROLE( display )
ITEMROLE( value )

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


Variant OldTestListItem::getData( int column, size_t roleId ) const
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


bool OldTestListItem::setData( int column, size_t roleId, const Variant& data )
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
	//std::vector<TestListItem::DataCallback> preChangeHandlers;
	//std::vector<TestListItem::DataCallback> postChangeHandlers;
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


TestListItem& TestListItem::operator=( const TestListItem& rhs )
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


Variant TestListItem::getData( int column, size_t roleId ) const
{
	if (roleId == ItemRole::displayId)
	{
		return column == 0 ? impl_->name_.c_str() : "Value";
	}
	else if (roleId == ItemRole::valueId)
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

	return Variant();
}


bool TestListItem::setData( int column, size_t roleId, const Variant & data )
{
	if (column == 1)
	{
		std::string string;
		size_t number;
		/*
		for (auto& event: impl_->preChangeHandlers)
		{
			event( column, roleId, data );
		}*/

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
		/*
		for (auto& event: impl_->postChangeHandlers)
		{
			event( column, roleId, data );
		}*/
	}

	return false;
}


Connection TestListItem::connectPreDataChanged( DataCallback callback )
{
	//impl_->dataToChangeEvent.push_back( callback );
	return Connection();
}


Connection TestListItem::connectPostDataChanged( DataCallback callback )
{
	//impl_->dataChangedEvent.push_back( callback );
	return Connection();
}
} // end namespace wgt
