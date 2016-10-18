#include "test_tree_model.hpp"
#include "test_tree_item.hpp"

#include "core_data_model/i_item_role.hpp"
#include "testing/data_model_test/test_data.hpp"

#include <unordered_map>
#include <vector>
#include <algorithm>
#include <array>
#include <string>
#include <memory>
#include <thread>

namespace wgt
{


struct TestTreeModelOld::Implementation
{
	Implementation( TestTreeModelOld& main );
	~Implementation();

	std::vector<TestTreeItemOld*> getSection( const TestTreeItemOld* parent );
	char* copyString( const std::string& s ) const;
	void generateData( const TestTreeItemOld* parent, size_t level );

	TestTreeModelOld& main_;
	std::unordered_map<const TestTreeItemOld*, std::vector<TestTreeItemOld*>> data_;
	StringList dataSource_;
	std::string headerText_;
	std::string footerText_;

	static const size_t NUMBER_OF_GROUPS = 5;
	static const size_t NUMBER_OF_LEVELS = 5;
};

TestTreeModelOld::Implementation::Implementation( TestTreeModelOld& main )
	: main_( main )
	, headerText_( "Random Words" )
	, footerText_( "The End" )
{
	generateData( nullptr, 0 );
}

TestTreeModelOld::Implementation::~Implementation()
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

std::vector<TestTreeItemOld*> TestTreeModelOld::Implementation::getSection(
	const TestTreeItemOld* parent )
{
	auto itr = data_.find( parent );
	assert( itr != data_.end() );
	return itr->second;
}

char* TestTreeModelOld::Implementation::copyString(
	const std::string& s ) const
{
	char* itemData = new char[s.length() + 1];
	memcpy( itemData, s.data(), s.length() );
	itemData[s.length()] = 0;
	return itemData;
}

void TestTreeModelOld::Implementation::generateData(
	const TestTreeItemOld* parent, size_t level )
{
	for (size_t i = 0; i < NUMBER_OF_GROUPS; ++i)
	{
		std::string dataString = dataSource_.next();
		TestTreeItemOld* item = new TestTreeItemOld(
			copyString( dataString ), parent );
		data_[parent].push_back( item );

		if (level < NUMBER_OF_LEVELS)
		{
			generateData( item, level + 1 );
		}

		data_[item];
	}
}


TestTreeModelOld::TestTreeModelOld()
	: impl_( new Implementation( *this ) )
{
}

TestTreeModelOld::TestTreeModelOld( const TestTreeModelOld& rhs )
	: impl_( new Implementation( *this ) )
{
}

TestTreeModelOld::~TestTreeModelOld()
{
}

TestTreeModelOld& TestTreeModelOld::operator=( const TestTreeModelOld& rhs )
{
	if (this != &rhs)
	{
		impl_.reset( new Implementation( *this ) );
	}

	return *this;
}

IItem* TestTreeModelOld::item( size_t index, const IItem* parent ) const
{
	auto temp = static_cast<const TestTreeItemOld*>( parent );
	return impl_->getSection( temp )[index];
}

ITreeModel::ItemIndex TestTreeModelOld::index( const IItem* item ) const
{
	auto temp = static_cast<const TestTreeItemOld*>( item );
	temp = static_cast<const TestTreeItemOld*>( temp->getParent() );
	ItemIndex index( 0, temp );

	auto items = impl_->getSection( temp );
	auto itr = std::find( items.begin(), items.end(), item );
	assert( itr != items.end() );

	index.first = itr - items.begin();
	return index;
}


bool TestTreeModelOld::empty( const IItem* parent ) const
{
	const auto temp = static_cast< const TestTreeItemOld* >( parent );
	return impl_->getSection( temp ).empty();
}


size_t TestTreeModelOld::size( const IItem* parent ) const
{
	auto temp = static_cast<const TestTreeItemOld*>( parent );
	return impl_->getSection( temp ).size();
}

int TestTreeModelOld::columnCount() const
{
	return 1;
}

Variant TestTreeModelOld::getData( int column, ItemRole::Id roleId ) const
{
	if (column >= columnCount())
	{
		return Variant();
	}

	auto role = static_cast< int >( roleId );
	if (role == headerTextRole::roleId_)
	{
		return impl_->headerText_.c_str();
	}
	else if (role == footerTextRole::roleId_)
	{
		return impl_->footerText_.c_str();
	}

	return Variant();
}

bool TestTreeModelOld::setData( int column, ItemRole::Id roleId, const Variant & data )
{
	return false;
}

void TestTreeModelOld::test()
{
	std::vector<TestTreeItemOld*> backup;

	//remove 3 leaf nodes.
	TestTreeItemOld* item = impl_->data_[nullptr][2];
	item = impl_->data_[item][2];
	item = impl_->data_[item][2];
	item = impl_->data_[item][2];
	item = impl_->data_[item][2];
	backup.emplace_back( impl_->data_[item][1] );
	backup.emplace_back( impl_->data_[item][2] );
	backup.emplace_back( impl_->data_[item][3] );

	signalPreItemsRemoved( item, 1, 3 );
	impl_->data_.erase( impl_->data_[item][1] );
	impl_->data_.erase( impl_->data_[item][2] );
	impl_->data_.erase( impl_->data_[item][3] );
	impl_->data_[item].erase(
		impl_->data_[item].begin() + 1, impl_->data_[item].begin() + 4 );
	signalPostItemsRemoved( item, 1, 3 );
	std::this_thread::sleep_for( std::chrono::milliseconds( 1500 ) );

	//remove 3 leaf nodes.
	item = impl_->data_[nullptr][2];
	item = impl_->data_[item][2];
	item = impl_->data_[item][2];
	item = impl_->data_[item][2];
	backup.emplace_back( impl_->data_[item][2] );
	backup.emplace_back( impl_->data_[impl_->data_[item][2]][0] );
	backup.emplace_back( impl_->data_[impl_->data_[item][2]][1] );

	signalPreItemsRemoved( item, 2, 1 );
	impl_->data_.erase( impl_->data_[impl_->data_[item][2]][0] );
	impl_->data_.erase( impl_->data_[impl_->data_[item][2]][1] );
	impl_->data_.erase( impl_->data_[item][2] );
	impl_->data_[item].erase( impl_->data_[item].begin() + 2 );
	signalPostItemsRemoved( item, 2, 1 );
	std::this_thread::sleep_for( std::chrono::milliseconds( 1500 ) );

	//insert a group node with 2 leaf nodes.
	item = impl_->data_[nullptr][2];
	item = impl_->data_[item][2];
	item = impl_->data_[item][2];
	item = impl_->data_[item][2];

	signalPreItemsInserted( item, 2, 1 );
	impl_->data_.emplace( backup[3], std::vector<TestTreeItemOld*>() );
	impl_->data_.emplace( backup[4], std::vector<TestTreeItemOld*>() );
	impl_->data_.emplace( backup[5], std::vector<TestTreeItemOld*>() );
	impl_->data_[item].insert( impl_->data_[item].begin() + 2, backup[3] );
	impl_->data_[impl_->data_[item][2]].emplace_back( backup[4] );
	impl_->data_[impl_->data_[item][2]].emplace_back( backup[5] );
	signalPostItemsInserted( item, 2, 1 );
	std::this_thread::sleep_for( std::chrono::milliseconds( 1500 ) );

	//insert 3 leaf nodes.
	item = impl_->data_[nullptr][2];
	item = impl_->data_[item][2];
	item = impl_->data_[item][2];
	item = impl_->data_[item][2];
	item = impl_->data_[item][2];

	signalPreItemsInserted( item, 1, 3 );
	impl_->data_.emplace( backup[0], std::vector<TestTreeItemOld*>() );
	impl_->data_.emplace( backup[1], std::vector<TestTreeItemOld*>() );
	impl_->data_.emplace( backup[2], std::vector<TestTreeItemOld*>() );
	impl_->data_[item].insert( impl_->data_[item].begin() + 1, backup[0] );
	impl_->data_[item].insert( impl_->data_[item].begin() + 2, backup[1] );
	impl_->data_[item].insert( impl_->data_[item].begin() + 3, backup[2] );
	signalPostItemsInserted( item, 1, 3 );
	std::this_thread::sleep_for( std::chrono::milliseconds( 1500 ) );

	item = impl_->data_[nullptr][2];
	const char* cdata = impl_->data_[item][2]->getDisplayText( 0 );
	char* data = const_cast<char*>( cdata );
	std::string newData = "xxxxxxxxxx";
	signalPreItemDataChanged( item, 0, ValueRole::roleId_, newData.data() );
	memcpy( data, newData.data(), newData.size() );
	signalPostItemDataChanged( item, 0, ValueRole::roleId_, newData.data() );
	std::this_thread::sleep_for( std::chrono::milliseconds( 1500 ) );

	newData = "comelxxxxx";
	signalPreItemDataChanged( item, 0, ValueRole::roleId_, newData.data() );
	memcpy( data, newData.data(), newData.size() );
	signalPostItemDataChanged( item, 0, ValueRole::roleId_, newData.data() );
	std::this_thread::sleep_for( std::chrono::milliseconds( 1500 ) );

	newData = "comeliness";
	signalPreItemDataChanged( item, 0, ValueRole::roleId_, newData.data() );
	memcpy( data, newData.data(), newData.size() );
	signalPostItemDataChanged( item, 0, ValueRole::roleId_, newData.data() );
}
} // end namespace wgt
