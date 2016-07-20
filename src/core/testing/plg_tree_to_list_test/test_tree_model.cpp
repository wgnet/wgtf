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


struct TestTreeModel::Implementation
{
	Implementation( TestTreeModel& main );
	~Implementation();

	std::vector<TestTreeItem*> getSection( const TestTreeItem* parent );
	char* copyString( const std::string& s ) const;
	void generateData( const TestTreeItem* parent, size_t level );

	TestTreeModel& main_;
	std::unordered_map<const TestTreeItem*, std::vector<TestTreeItem*>> data_;
	StringList dataSource_;

	static const size_t NUMBER_OF_GROUPS = 5;
	static const size_t NUMBER_OF_LEVELS = 5;
};

TestTreeModel::Implementation::Implementation( TestTreeModel& main )
	: main_( main )
{
	generateData( nullptr, 0 );
}

TestTreeModel::Implementation::~Implementation()
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

std::vector<TestTreeItem*> TestTreeModel::Implementation::getSection(
	const TestTreeItem* parent )
{
	auto itr = data_.find( parent );
	assert( itr != data_.end() );
	return itr->second;
}

char* TestTreeModel::Implementation::copyString(
	const std::string& s ) const
{
	char* itemData = new char[s.length() + 1];
	memcpy( itemData, s.data(), s.length() );
	itemData[s.length()] = 0;
	return itemData;
}

void TestTreeModel::Implementation::generateData(
	const TestTreeItem* parent, size_t level )
{
	for (size_t i = 0; i < NUMBER_OF_GROUPS; ++i)
	{
		std::string dataString = dataSource_.next();
		TestTreeItem* item = new TestTreeItem(
			copyString( dataString ), parent );
		data_[parent].push_back( item );

		if (level < NUMBER_OF_LEVELS)
		{
			generateData( item, level + 1 );
		}

		data_[item];
	}
}


TestTreeModel::TestTreeModel()
	: impl_( new Implementation( *this ) )
{
}

TestTreeModel::TestTreeModel( const TestTreeModel& rhs )
	: impl_( new Implementation( *this ) )
{
}

TestTreeModel::~TestTreeModel()
{
}

TestTreeModel& TestTreeModel::operator=( const TestTreeModel& rhs )
{
	if (this != &rhs)
	{
		impl_.reset( new Implementation( *this ) );
	}

	return *this;
}

IItem* TestTreeModel::item( size_t index, const IItem* parent ) const
{
	auto temp = static_cast<const TestTreeItem*>( parent );
	return impl_->getSection( temp )[index];
}

ITreeModel::ItemIndex TestTreeModel::index( const IItem* item ) const
{
	auto temp = static_cast<const TestTreeItem*>( item );
	temp = static_cast<const TestTreeItem*>( temp->getParent() );
	ItemIndex index( 0, temp );

	auto items = impl_->getSection( temp );
	auto itr = std::find( items.begin(), items.end(), item );
	assert( itr != items.end() );

	index.first = itr - items.begin();
	return index;
}


bool TestTreeModel::empty( const IItem* parent ) const
{
	const auto temp = static_cast< const TestTreeItem* >( parent );
	return impl_->getSection( temp ).empty();
}


size_t TestTreeModel::size( const IItem* parent ) const
{
	auto temp = static_cast<const TestTreeItem*>( parent );
	return impl_->getSection( temp ).size();
}

int TestTreeModel::columnCount() const
{
	return 1;
}
} // end namespace wgt
