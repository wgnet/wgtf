#include "pch.hpp"
#include "test_data_model_objects.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/filtered_list_model.hpp"
#include "core_data_model/variant_list.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_unit_test/unit_test.hpp"

namespace wgt
{
//---------------------------------------------------------------------------
// Test Fixture Class
//---------------------------------------------------------------------------

void TestFixture::initialise( TestStringData::State state )
{
	testStringData_.initialise( state );
}

//---------------------------------------------------------------------------
// Test Tree Model
//---------------------------------------------------------------------------

struct UnitTestTreeItem::Implementation
{
	Implementation( UnitTestTreeItem& main, const char* name, const IItem* parent );
	~Implementation();

	UnitTestTreeItem& main_;
	const char* name_;
	const IItem* parent_;
};

UnitTestTreeItem::Implementation::Implementation(
	UnitTestTreeItem& main, const char* name, const IItem* parent )
	: main_( main )
	, name_( name )
	, parent_( parent )
{
}

UnitTestTreeItem::Implementation::~Implementation()
{
	delete[] name_;
}


UnitTestTreeItem::UnitTestTreeItem( const char* name, const IItem* parent )
	: impl_( new Implementation( *this, name, parent ) )
{
}

UnitTestTreeItem::UnitTestTreeItem( const UnitTestTreeItem& rhs )
	: impl_( new Implementation( *this, rhs.impl_->name_, rhs.impl_->parent_ ) )
{
}

UnitTestTreeItem::~UnitTestTreeItem()
{
}

UnitTestTreeItem& UnitTestTreeItem::operator=( const UnitTestTreeItem& rhs )
{
	if (this != &rhs)
	{
		impl_.reset( new Implementation(
			*this, rhs.impl_->name_, rhs.impl_->parent_ ) );
	}

	return *this;
}

const IItem* UnitTestTreeItem::getParent() const
{
	return impl_->parent_;
}

const char* UnitTestTreeItem::getDisplayText( int column ) const
{
	return impl_->name_;
}

ThumbnailData UnitTestTreeItem::getThumbnail( int column ) const
{
	return nullptr;
}

void UnitTestTreeItem::setName( const char * name )
{
	if (impl_->name_ != nullptr)
	{
		delete[] impl_->name_;
	}

	impl_->name_ = name;
}

Variant UnitTestTreeItem::getData( int column, size_t roleId ) const
{
	return Variant();
}

bool UnitTestTreeItem::setData( int column, size_t roleId, const Variant& data )
{
	return true;
}

struct UnitTestTreeModel::Implementation
{
	Implementation( UnitTestTreeModel& main );
	~Implementation();

	void initialise( TestStringData * dataSource );

	std::vector<UnitTestTreeItem*> getSection( const UnitTestTreeItem* parent );
	char* copyString( const std::string& s ) const;
	void generateData( const UnitTestTreeItem* parent, size_t level );

	UnitTestTreeModel& model_;
	std::unordered_map<const UnitTestTreeItem*, std::vector<UnitTestTreeItem*>> data_;
	TestStringData* dataSource_;

	static const size_t NUMBER_OF_GROUPS = 3;
	static const size_t NUMBER_OF_LEVELS = 2;
};

UnitTestTreeModel::Implementation::Implementation( UnitTestTreeModel& model )
	: model_( model )
	, dataSource_( nullptr )
{
}

void UnitTestTreeModel::Implementation::initialise( TestStringData * dataSource )
{
	dataSource_ = dataSource;
	generateData( nullptr, 0 );
}

UnitTestTreeModel::Implementation::~Implementation()
{
	for (auto itr = data_.begin(); itr != data_.end(); ++itr)
	{
		auto items = itr->second;
		size_t max = items.size();

		for (size_t i = 0; i < max; ++i)
		{
			delete items[i];
		}
	}

	data_.clear();
}

std::vector<UnitTestTreeItem*> UnitTestTreeModel::Implementation::getSection( 
	const UnitTestTreeItem* parent )
{
	auto itr = data_.find( parent );
	assert( itr != data_.end() );
	return itr->second;
}

char* UnitTestTreeModel::Implementation::copyString( const std::string& s ) const
{
	char* itemData = new char[s.length() + 1];
	memcpy( itemData, s.data(), s.length() );
	itemData[s.length()] = 0;
	return itemData;
}

void UnitTestTreeModel::Implementation::generateData( const UnitTestTreeItem* parent, size_t level )
{
	for (size_t i = 0; i < NUMBER_OF_GROUPS; ++i)
	{
		std::string dataString = dataSource_->getNextString();
		UnitTestTreeItem* item = new UnitTestTreeItem( copyString( dataString ), parent );
		data_[parent].push_back( item );

		if (level < NUMBER_OF_LEVELS)
		{
			generateData( item, level + 1 );
		}

		data_[item];
	}
}

UnitTestTreeModel::UnitTestTreeModel()
	: impl_( new Implementation( *this ) )
{
}

UnitTestTreeModel::UnitTestTreeModel( const UnitTestTreeModel& rhs )
	: impl_( new Implementation( *this ) )
{
}

UnitTestTreeModel::~UnitTestTreeModel()
{
}

UnitTestTreeModel& UnitTestTreeModel::operator=( const UnitTestTreeModel& rhs )
{
	if (this != &rhs)
	{
		impl_.reset( new Implementation( *this ) );
	}

	return *this;
}

void UnitTestTreeModel::initialise( TestStringData * dataSource )
{
	impl_->initialise( dataSource );
}

IItem* UnitTestTreeModel::item( size_t index, const IItem* parent ) const
{
	auto temp = static_cast<const UnitTestTreeItem*>( parent );
	if ( size( parent ) > 0 )
	{
		return impl_->getSection( temp )[index];
	}

	return nullptr;
}

ITreeModel::ItemIndex UnitTestTreeModel::index( const IItem* item ) const
{
	auto temp = static_cast<const UnitTestTreeItem*>( item );
	temp = static_cast<const UnitTestTreeItem*>( temp->getParent() );
	ItemIndex index( 0, temp );

	auto items = impl_->getSection( temp );
	auto itr = std::find( items.begin(), items.end(), item );
	assert( itr != items.end() );

	index.first = itr - items.begin();
	return index;
}

bool UnitTestTreeModel::empty( const IItem* parent ) const
{
	const auto temp = static_cast< const UnitTestTreeItem* >( parent );
	return impl_->getSection( temp ).empty();
}

size_t UnitTestTreeModel::size( const IItem* parent ) const
{
	auto temp = static_cast<const UnitTestTreeItem*>( parent );
	return impl_->getSection( temp ).size();
}

int UnitTestTreeModel::columnCount() const
{
	return 1;
}

UnitTestTreeItem * UnitTestTreeModel::insert( const UnitTestTreeItem * parent, std::string & data, InsertAt where )
{
	size_t index = where == InsertAt::BACK ? size( parent ) : 0;
	signalPreItemsInserted( parent, index, 1 );
	
	UnitTestTreeItem* item = new UnitTestTreeItem( impl_->copyString( data ), parent );
	impl_->data_.emplace( item, std::vector< UnitTestTreeItem * >() );

	if (where == InsertAt::FRONT)
	{
		auto& items = impl_->data_[parent];
		items.insert( items.begin(), item );
	}
	else
	{
		impl_->data_[parent].push_back( item );
	}

	signalPostItemsInserted( parent, index, 1 );
	return item;
}

void UnitTestTreeModel::erase( size_t index, const UnitTestTreeItem * parent )
{
	signalPreItemsRemoved( parent, index, 1 );

	// Remove this item's children first
	auto subItem = item( index, parent );
	unsigned int children = static_cast< unsigned int >( size( subItem ) );
	for (unsigned int i = 0; i < children; ++i)
	{		
		impl_->data_.erase( impl_->data_[impl_->data_[parent][index]][i] );
	}

	// Now remove the item
	impl_->data_.erase( impl_->data_[parent][index] );
	impl_->data_[parent].erase( impl_->data_[parent].begin() + index );

	signalPostItemsRemoved( parent, index, 1 );
}

void UnitTestTreeModel::update( size_t index, const UnitTestTreeItem * parent, std::string & data )
{
	auto treeItem = item( index, parent );
	if (treeItem == nullptr)
	{
		return;
	}

	auto unitTestTreeItem = dynamic_cast< UnitTestTreeItem * >( treeItem );
	if (unitTestTreeItem == nullptr)
	{
		return;
	}

	signalPreItemDataChanged( treeItem, 0, ValueRole::roleId_, data );

	unitTestTreeItem->setName( impl_->copyString( data ) );
	
	signalPostItemDataChanged( treeItem, 0, ValueRole::roleId_, data );
}


//---------------------------------------------------------------------------
// List Model Helper Functions
//---------------------------------------------------------------------------

void TestFixture::echoListData()
{
	// Debug output for engineers verifying list contents
	const VariantList & list = testStringData_.getVariantList();
	std::string value;
	for (unsigned int i = 0; i < static_cast< unsigned int >( list.size() ); ++i)
	{
		auto variant = list[i];
		if (!variant.typeIs< const char * >() && !variant.typeIs< std::string >())
		{
			continue;
		}

		value = "";
		if (variant.tryCast( value ))
		{
			BWUnitTest::unitTestInfo( "List Item %s\n", value.c_str() );
		}
	}
}

bool TestFixture::findItemInFilteredList( const char * term, bool exactMatch )
{
	if (term == nullptr)
	{
		return false;
	}
		
	std::string value;
	for (unsigned int i = 0; i < static_cast< unsigned int >( filteredTestList_.size() ); ++i)
	{
		auto item = filteredTestList_.item( i );
		assert(item);
		Variant variant = item->getData( 0, ValueRole::roleId_ );
		if (!variant.typeIs< const char * >() && !variant.typeIs< std::string >())
		{
			return false;
		}

		value = "";
		if (variant.tryCast( value ))
		{
			if ( (exactMatch && value.compare( term ) == 0) || 
				 (!exactMatch && value.find( term ) != std::string::npos) )
			{
				return true;
			}
		}
	}

	return false;
}

bool TestFixture::verifyListItemPosition( unsigned int index, const char * value )
{
	if (value == nullptr)
	{
		return false;
	}

	const VariantList & list = testStringData_.getVariantList();
		
	std::string itemValue;
	auto item = list.item( index );
	auto variant = item->getData( 0, ValueRole::roleId_ );
	if (!variant.typeIs< const char * >() && !variant.typeIs< std::string >())
	{
		return false;
	}

	if (variant.tryCast( itemValue ) && itemValue.find( value ) != std::string::npos)
	{
		return true;
	}

	return false;
}

void TestFixture::insertIntoListAtIndex( unsigned int index, const char * value )
{	
	unsigned int tracker = 0;
	VariantList & list = testStringData_.getVariantList();

	for (auto it = list.begin(); it != list.end(); ++it)
	{
		if (tracker == index)
		{
			list.insert( it, value );
			return;
		}

		++tracker;
	}
}

void TestFixture::removeFromListAtIndex( unsigned int index )
{
	unsigned int tracker = 0;
	VariantList & list = testStringData_.getVariantList();

	for (auto it = list.begin(); it != list.end(); ++it)
	{
		if (tracker == index)
		{
			list.erase( it );
			return;
		}

		++tracker;
	}
}

void TestFixture::getListItemValueAtIndex( unsigned int index, std::string & value )
{
	const VariantList & list = testStringData_.getVariantList();
		
	auto item = list.item( index );
	assert( item );

	auto variant = item->getData( 0, ValueRole::roleId_ );
	if (!variant.typeIs< const char * >() && !variant.typeIs< std::string >())
	{
		value = "";
		return;
	}

	variant.tryCast( value );
}

void TestFixture::updateListItemAtIndex( unsigned int index, const char * value )
{
	VariantList & list = testStringData_.getVariantList();
	
	auto item = list.item( index );
	assert( item );

	list.signalPreItemDataChanged( item, 0, ValueRole::roleId_, value );

	item->setData( 0, ValueRole::roleId_, value );

	list.signalPostItemDataChanged( item, 0, ValueRole::roleId_, value );
}


//---------------------------------------------------------------------------
// Tree Model Helper Functions
//---------------------------------------------------------------------------

bool TestFixture::verifyTreeItemMatch( IItem * item, const char * value, bool exactMatch )
{
	if (item == nullptr || value == nullptr)
	{
		return false;
	}

	std::string displayText = item->getDisplayText( 0 );
	if ( (exactMatch && displayText.compare( value ) == 0) ||
		 (!exactMatch && displayText.find( value ) != std::string::npos) )
	{
		return true;
	}

	return false;
}
} // end namespace wgt
