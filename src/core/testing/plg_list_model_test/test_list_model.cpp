#include "test_list_model.hpp"
#include "test_list_item.hpp"

#include "core_data_model/i_item_role.hpp"
#include "testing/data_model_test/test_data.hpp"

#include <unordered_map>
#include <vector>
#include <algorithm>
#include <array>
#include <string>
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


Variant OldTestListModel::getData( int column, size_t roleId ) const
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


bool OldTestListModel::setData( int column, size_t roleId, const Variant & data )
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
	Signal< TestListModel::DataSignature > preDataChanged_;
	Signal< TestListModel::DataSignature > postDataChanged_;
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
			size_t role,
			const Variant & value )
		{
			preModelDataChanged( row, column, role, value );
		};
		const auto preDataChanged = pItem->connectPreDataChanged( preData );

		const auto & postModelDataChanged = postDataChanged_;
		const auto postData = [ row, &postModelDataChanged ]( int column,
			size_t role,
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


Variant TestListModel::getData( int row, int column, size_t roleId ) const
{
	if (column >= columnCount())
	{
		return Variant();
	}

	// Header/footer data only
	// This function is only for data associated with the model, not items
	auto role = static_cast< int >( roleId );
	if (role == headerTextRole::roleId_)
	{
		return impl_->headerText_[column].c_str();
	}
	else if (role == footerTextRole::roleId_)
	{
		return impl_->footerText_[column].c_str();
	}

	return AbstractListModel::getData( row, column, roleId );
}


bool TestListModel::setData( int row, int column, size_t roleId, const Variant & data )
{
	// Header/footer data only
	// This function is only for data associated with the model, not items
	return AbstractListModel::setData( row, column, roleId, data );
}


AbstractItem * TestListModel::item( int row ) const
{
	return impl_->items_.at( size_t( row ) );
}


int TestListModel::index( const AbstractItem * item ) const
{
	auto& items = impl_->items_;
	auto itr = std::find( items.begin(), items.end(), item );
	return itr == items.end() ? -1 : int( itr - items.begin() );
}


int TestListModel::rowCount() const
{
	return (int)impl_->items_.size();
}


int TestListModel::columnCount() const
{
	return impl_->columnCount();
}


Connection TestListModel::connectPreItemDataChanged( DataCallback callback ) /* override */
{
	return impl_->preDataChanged_.connect( callback );
}


Connection TestListModel::connectPostItemDataChanged( DataCallback callback ) /* override */
{
	return impl_->postDataChanged_.connect( callback );
}


} // end namespace wgt
