#include "test_grid_model.hpp"
#include "test_grid_item.hpp"

#include "core_data_model/i_item_role.hpp"

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

void TestGridModel::generateData()
{
	std::string dataString = dataSource_.next();
	std::random_device randomDevice;
	std::default_random_engine randomEngine( randomDevice() );
	std::uniform_int_distribution<size_t> uniformDistribution( 0, 999999 );
	size_t max = 1000;

	std::vector< TestGridItem * > items;
	while (!dataString.empty())
	{
		items_.push_back( new TestGridItem( dataString.c_str() ) );

		dataString = dataSource_.next();

		if (--max == 0)
		{
			break;
		}
	}

	for (auto i = 0; i < columnCount(); ++i)
	{
		char buffer[8];
		sprintf( buffer, "Header%d", i );
		headerText_.push_back( buffer );
		sprintf( buffer, "Footer%d", i );
		footerText_.push_back( buffer );
	}
}

void TestGridModel::clear()
{
	for (auto& item: items_)
	{
		delete item;
	}

	items_.clear();
	headerText_.clear();
	footerText_.clear();
}


TestGridModel::TestGridModel()
{
	generateData();
}


TestGridModel::~TestGridModel()
{
	clear();
}


Variant TestGridModel::getData( int row, int column, size_t roleId ) const
{
	if (column >= columnCount())
	{
		return Variant();
	}

	auto role = static_cast< int >( roleId );
	if (role == headerTextRole::roleId_)
	{
		return headerText_[column].c_str();
	}
	else if (role == footerTextRole::roleId_)
	{
		return footerText_[column].c_str();
	}

	return Variant();
}


bool TestGridModel::setData( int row, int column, size_t roleId, const Variant & data )
{
	return false;
}


AbstractItem * TestGridModel::item( const ItemIndex & index ) const
{
	auto i = index.row_ * columnCount() + index.column_;
	return items_.at( size_t( i ) );
}


AbstractTableModel::ItemIndex TestGridModel::index( const AbstractItem * item ) const
{
	auto columns = columnCount();
	if (columns == 0)
	{
		return ItemIndex();
	}
	auto& items = items_;
	auto itr = std::find( items.begin(), items.end(), item );
	if (itr == items.end())
	{
		return ItemIndex();
	}
	auto i = int( itr - items.begin() );
	return ItemIndex( i / columns, i % columns );
}


int TestGridModel::rowCount() const
{
	auto columns = columnCount();
	if (columns == 0)
	{
		return 0;
	}

	return (int)( items_.size() + columns - 1 ) / columns;
}


int TestGridModel::columnCount() const
{
	return 4;
}
} // end namespace wgt
