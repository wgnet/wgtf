#include "test_table_model.hpp"
#include "test_table_item.hpp"

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


void TestTableModel::generateData()
{
	std::string dataString = dataSource_.next();
	std::random_device randomDevice;
	std::default_random_engine randomEngine( randomDevice() );
	std::uniform_int_distribution<size_t> uniformDistribution( 0, 999999 );
	size_t max = 1000;

	std::vector< TestTableItem * > items;
	while (!dataString.empty())
	{
		items_.push_back( new TestTableItem( dataString.c_str() ) );

		auto & pItem = items_.back();

		const auto index = this->index( pItem );
		const auto & preModelDataChanged = preDataChanged_;
		const auto preData = [ index, &preModelDataChanged ](
			size_t role,
			const Variant & newValue )
		{
			preModelDataChanged( index, role, newValue );
		};
		const auto preDataChanged = pItem->connectPreDataChanged( preData );

		const auto & postModelDataChanged = postDataChanged_;
		const auto postData = [ index, &postModelDataChanged ](
			size_t role,
			const Variant & newValue )
		{
			postModelDataChanged( index, role, newValue );
		};
		const auto postDataChanged = pItem->connectPostDataChanged( postData );
		
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


void TestTableModel::clear()
{
	for (auto& item: items_)
	{
		delete item;
	}

	items_.clear();
	headerText_.clear();
	footerText_.clear();
}


TestTableModel::TestTableModel()
{
	generateData();
}


TestTableModel::~TestTableModel()
{
	clear();
}


Variant TestTableModel::getData( int row, int column, size_t roleId ) const
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
		return headerText_[column].c_str();
	}
	else if (role == footerTextRole::roleId_)
	{
		return footerText_[column].c_str();
	}

	return AbstractTableModel::getData( row, column, roleId );
}


bool TestTableModel::setData( int row, int column, size_t roleId, const Variant & data )
{
	// Header/footer data only
	// This function is only for data associated with the model, not items
	return AbstractTableModel::setData( row, column, roleId, data );
}


AbstractItem * TestTableModel::item( const ItemIndex & index ) const
{
	auto i = index.row_ * columnCount() + index.column_;
	return items_.at( size_t( i ) );
}


AbstractTableModel::ItemIndex TestTableModel::index( const AbstractItem * item ) const
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


int TestTableModel::rowCount() const
{
	auto columns = columnCount();
	if (columns == 0)
	{
		return 0;
	}

	return (int)( items_.size() + columns - 1 ) / columns;
}


int TestTableModel::columnCount() const
{
	return 4;
}


Connection TestTableModel::connectPreItemDataChanged( DataCallback callback ) /* override */
{
	return preDataChanged_.connect( callback );
}


Connection TestTableModel::connectPostItemDataChanged( DataCallback callback ) /* override */
{
	return postDataChanged_.connect( callback );
}


} // end namespace wgt
