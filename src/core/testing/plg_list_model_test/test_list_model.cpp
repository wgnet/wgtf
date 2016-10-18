#include "test_list_model.hpp"
#include "test_list_item.hpp"

#include "core_data_model/i_item_role.hpp"
#include "core_data_model/common_data_roles.hpp"
#include "testing/data_model_test/test_data.hpp"

#include <unordered_map>
#include <vector>
#include <algorithm>
#include <array>
#include <string>
#include <sstream>
#include <memory>
#include <thread>
#include <random>

namespace wgt
{

struct OldTestListModel::Implementation
{
	Implementation( OldTestListModel& self, bool shortList );
	~Implementation();

	char* copyString( const std::string& s ) const;
	void generateData();
	void clear();
	int columnCount();

	OldTestListModel& self_;
	std::vector<OldTestListItem*> items_;
	StringList2 dataSource_;
	bool shortList_;
	std::vector<std::string> headerText_;
	std::vector<std::string> footerText_;
};


OldTestListModel::Implementation::Implementation( OldTestListModel& self, bool shortList )
	: self_( self )
	, shortList_( shortList )
{
	generateData();

	headerText_.push_back( "Random Words" );

	if (columnCount() == 2)
	{
		headerText_.push_back( "Second Column" );
		footerText_.push_back( "The" );
		footerText_.push_back( "End" );
	}
	else
	{
		footerText_.push_back( "The End" );
	}
}


OldTestListModel::Implementation::~Implementation()
{
	clear();
}


char* OldTestListModel::Implementation::copyString(
	const std::string& s ) const
{
	char* itemData = new char[s.length() + 1];
	memcpy( itemData, s.data(), s.length() );
	itemData[s.length()] = 0;
	return itemData;
}


void OldTestListModel::Implementation::generateData()
{
	std::string dataString = dataSource_.next();
	std::random_device randomDevice;
	std::default_random_engine randomEngine( randomDevice() );
	std::uniform_int_distribution<size_t> uniformDistribution( 0, 999999 );
	size_t max = shortList_ ? 1000 : 2000;

	while (!dataString.empty())
	{
		if (!shortList_ && items_.size() % 3 == 0)
		{
			size_t colour = uniformDistribution( randomEngine );
			items_.push_back( new OldTestListItem(
				dataString.c_str(), colour ) );
		}
		else
		{
			items_.push_back( new OldTestListItem(
				dataString.c_str(), dataString.c_str() ) );
		}
		
		dataString = dataSource_.next();

		if (--max == 0)
		{
			break;
		}
	}
}


void OldTestListModel::Implementation::clear()
{
	for (auto& item: items_)
	{
		delete item;
	}

	items_.clear();
}


int OldTestListModel::Implementation::columnCount()
{
	return shortList_ ? 1 : 2;
}


OldTestListModel::OldTestListModel( bool shortList )
	: impl_( new Implementation( *this, shortList ) )
{
}


OldTestListModel::OldTestListModel( const OldTestListModel& rhs )
	: impl_( new Implementation( *this, rhs.impl_->shortList_ ) )
{
}


OldTestListModel::~OldTestListModel()
{
}


OldTestListModel& OldTestListModel::operator=( const OldTestListModel& rhs )
{
	if (this != &rhs)
	{
		impl_.reset( new Implementation( *this, rhs.impl_->shortList_ ) );
	}

	return *this;
}


IItem* OldTestListModel::item( size_t index ) const
{
	return impl_->items_.at( index );
}


size_t OldTestListModel::index( const IItem* item ) const
{
	auto& items = impl_->items_;
	auto itr = std::find( items.begin(), items.end(), item );
	return itr == items.end() ? -1 : itr - items.begin();
}


bool OldTestListModel::empty() const
{
	return impl_->items_.empty();
}


size_t OldTestListModel::size() const
{
	return impl_->items_.size();
}


int OldTestListModel::columnCount() const
{
	return impl_->columnCount();
}


Variant OldTestListModel::getData( int column, ItemRole::Id roleId ) const
{
	if (column >= columnCount())
	{
		return Variant();
	}

	if (roleId == headerTextRole::roleId_)
	{
		return impl_->headerText_[column].c_str();
	}
	else if (roleId == footerTextRole::roleId_)
	{
		return impl_->footerText_[column].c_str();
	}

	return Variant();
}


bool OldTestListModel::setData( int column, ItemRole::Id roleId, const Variant & data )
{
	return false;
}


/////////////////////////////////////////////////////////
struct TestListModel::Implementation
{
	Implementation( TestListModel& self, bool shortList );
	~Implementation();

	char* copyString( const std::string& s ) const;
	void generateData();
	void clear();
	int columnCount();

	TestListModel& self_;
	std::vector<TestListItem*> items_;
	StringList2 dataSource_;
	bool shortList_;
	std::vector<std::string> headerText_;
	std::vector<std::string> footerText_;

	Signal< AbstractListModel::DataSignature > preDataChanged_;
	Signal< AbstractListModel::DataSignature > postDataChanged_;

	Signal< AbstractListModel::MoveSignature > preRowsMoved_;
	Signal< AbstractListModel::MoveSignature > postRowsMoved_;
	Signal< AbstractListModel::RangeSignature > preRowsInserted_;
	Signal< AbstractListModel::RangeSignature > postRowsInserted_;
	Signal< AbstractListModel::RangeSignature > preRowsRemoved_;
	Signal< AbstractListModel::RangeSignature > postRowsRemoved_;
};


TestListModel::Implementation::Implementation( TestListModel& self, bool shortList )
	: self_( self )
	, shortList_( shortList )
{
	generateData();

	headerText_.push_back( "Random Words" );

	if (columnCount() == 2)
	{
		headerText_.push_back( "Second Column" );
		footerText_.push_back( "The" );
		footerText_.push_back( "End" );
	}
	else
	{
		footerText_.push_back( "The End" );
	}
}


TestListModel::Implementation::~Implementation()
{
	clear();
}


char* TestListModel::Implementation::copyString(
	const std::string& s ) const
{
	char* itemData = new char[s.length() + 1];
	memcpy( itemData, s.data(), s.length() );
	itemData[s.length()] = 0;
	return itemData;
}


void TestListModel::Implementation::generateData()
{
	std::string dataString = dataSource_.next();
	std::random_device randomDevice;
	std::default_random_engine randomEngine( randomDevice() );
	std::uniform_int_distribution<size_t> uniformDistribution( 0, 999999 );
	size_t max = shortList_ ? 100 : 1000;

	while (!dataString.empty())
	{
		if (!shortList_ && items_.size() % 3 == 0)
		{
			size_t colour = uniformDistribution( randomEngine );
			items_.push_back( new TestListItem(
				dataString.c_str(), colour ) );
		}
		else
		{
			items_.push_back( new TestListItem(
				dataString.c_str(), dataString.c_str() ) );
		}

		auto & pItem = items_.back();

		const int row = static_cast< int >( items_.size() - 1 );
		const auto & preModelDataChanged = preDataChanged_;
		const auto preData = [ row, &preModelDataChanged ]( int column,
			ItemRole::Id role,
			const Variant & value )
		{
			preModelDataChanged( row, column, role, value );
		};
		const auto preDataChanged = pItem->connectPreDataChanged( preData );

		const auto & postModelDataChanged = postDataChanged_;
		const auto postData = [ row, &postModelDataChanged ]( int column,
			ItemRole::Id role,
			const Variant & value )
		{
			postModelDataChanged( row, column, role, value );
		};
		const auto postDataChanged = pItem->connectPostDataChanged( postData );
		
		dataString = dataSource_.next();

		if (--max == 0)
		{
			break;
		}
	}
}


void TestListModel::Implementation::clear()
{
	for (auto& item: items_)
	{
		delete item;
	}

	items_.clear();
}


int TestListModel::Implementation::columnCount()
{
	return shortList_ ? 1 : 2;
}


TestListModel::TestListModel( bool shortList )
	: impl_( new Implementation( *this, shortList ) )
{
}


TestListModel::TestListModel( const TestListModel& rhs )
	: impl_( new Implementation( *this, rhs.impl_->shortList_ ) )
{
}


TestListModel::~TestListModel()
{
}


TestListModel& TestListModel::operator=( const TestListModel& rhs )
{
	if (this != &rhs)
	{
		impl_.reset( new Implementation( *this, rhs.impl_->shortList_ ) );
	}

	return *this;
}


Variant TestListModel::getData( int row, int column, ItemRole::Id roleId ) const
{
	if ((column < 0) || (column >= this->columnCount()))
	{
		return Variant();
	}

	// Header/footer data only
	// This function is only for data associated with the model, not items
	if (roleId == headerTextRole::roleId_)
	{
		return impl_->headerText_[column].c_str();
	}
	else if (roleId == footerTextRole::roleId_)
	{
		return impl_->footerText_[column].c_str();
	}

	return AbstractListModel::getData( row, column, roleId );
}


bool TestListModel::setData( int row, int column, ItemRole::Id roleId, const Variant & data )
{
	if ((column < 0) || (column >= this->columnCount()))
	{
		return false;
	}

	// Header/footer data only
	// This function is only for data associated with the model, not items
	if (roleId == headerTextRole::roleId_)
	{
		impl_->preDataChanged_( row, column, roleId, data );
		const auto result = data.tryCast( impl_->headerText_[ column ] );
		impl_->postDataChanged_( row, column, roleId, data );
		return result;
	}
	else if (roleId == footerTextRole::roleId_)
	{
		impl_->preDataChanged_( row, column, roleId, data );
		const auto result = data.tryCast( impl_->footerText_[ column ] );
		impl_->postDataChanged_( row, column, roleId, data );
		return result;
	}

	return AbstractListModel::setData( row, column, roleId, data );
}


AbstractItem * TestListModel::item( int row ) const
{
	if (row >= 0 && row < (int)impl_->items_.size())
	{
		return impl_->items_.at( size_t( row ) );
	}

	return nullptr;
}


int TestListModel::index( const AbstractItem * item ) const
{
	auto& items = impl_->items_;
	auto itr = std::find( items.begin(), items.end(), item );

	int result = 0;
	
	if (itr == items.end())
	{
		result = -1;
	}
	else
	{
		result = int( itr - items.begin() );
	}

	return result;
}


int TestListModel::rowCount() const
{
	int result = (int)impl_->items_.size();
	return result;
}


int TestListModel::columnCount() const
{
	return impl_->columnCount();
}


bool TestListModel::insertRows( int row, int count ) /* override */
{
	this->impl_->preRowsInserted_( row, count );

	auto & items = impl_->items_;

	for (int i = 0; i < count; ++i)
	{
		items.insert( items.begin() + row, new TestListItem( "new row", "new row" ) );
	}

	this->impl_->postRowsInserted_( row, count );
	return true;
}


bool TestListModel::removeRows( int row, int count ) /* override */
{
	this->impl_->preRowsRemoved_( row, count );

	auto & items = impl_->items_;
	const auto firstItr = items.cbegin() + row;
	const auto lastItr = firstItr + count;

	for (auto i = firstItr; i != lastItr; ++i)
	{
		delete *i;
	}

	items.erase( firstItr, lastItr );
	
	this->impl_->postRowsRemoved_( row, count );

	return true;
}

bool TestListModel::moveRows( int sourceRow, int count, int destinationRow )
{
	if (destinationRow >= sourceRow && destinationRow <= sourceRow + count)
	{
		return false;
	}

	impl_->preRowsMoved_( sourceRow, sourceRow + count - 1, destinationRow );

	std::vector<TestListItem*> movedItems;
	auto & items = impl_->items_;

	for (int i = sourceRow; i < sourceRow + count; ++i)
	{
		movedItems.push_back( items[i] );
	}

	const auto firstItr = items.cbegin() + sourceRow;
	const auto lastItr = firstItr + count;
	items.erase( firstItr, lastItr );

	if (sourceRow + count < destinationRow)
	{
		destinationRow -= count;
	}

	items.insert( items.begin() + destinationRow, movedItems.begin(), movedItems.end() );

	impl_->postRowsMoved_( sourceRow, sourceRow + count - 1, destinationRow );

	return true;
}


std::vector< std::string > TestListModel::roles() const
{
	std::vector< std::string > roles;
	roles.push_back( ItemRole::valueName );
	roles.push_back( ItemRole::headerTextName );
	roles.push_back( ItemRole::footerTextName );
	return roles;
}


Connection TestListModel::connectPreItemDataChanged( DataCallback callback ) /* override */
{
	return impl_->preDataChanged_.connect( callback );
}


Connection TestListModel::connectPostItemDataChanged( DataCallback callback ) /* override */
{
	return impl_->postDataChanged_.connect( callback );
}

Connection TestListModel::connectPreRowsMoved( MoveCallback callback )
{
	return impl_->preRowsMoved_.connect( callback );
}

Connection TestListModel::connectPostRowsMoved( MoveCallback callback )
{
	return impl_->postRowsMoved_.connect( callback );
}

Connection TestListModel::connectPreRowsInserted( RangeCallback callback )
{
	return impl_->preRowsInserted_.connect( callback );
}

Connection TestListModel::connectPostRowsInserted( RangeCallback callback )
{
	return impl_->postRowsInserted_.connect( callback );
}

Connection TestListModel::connectPreRowsRemoved( RangeCallback callback )
{
	return impl_->preRowsRemoved_.connect( callback );
}

Connection TestListModel::connectPostRowsRemoved( RangeCallback callback )
{
	return impl_->postRowsRemoved_.connect( callback );
}


} // end namespace wgt
