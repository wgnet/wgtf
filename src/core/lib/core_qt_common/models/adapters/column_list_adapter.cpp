#include "column_list_adapter.hpp"

#include <cassert>


namespace wgt
{
ColumnListAdapter::ColumnListAdapter( const QModelIndex & row )
	: model_( row.model() )
	, row_( row )
{
	assert( row.isValid() );
	connect();
}

ColumnListAdapter::~ColumnListAdapter()
{
	disconnect();
}

QAbstractItemModel * ColumnListAdapter::model() const
{
	return const_cast< QAbstractItemModel * >( model_ );
}

QModelIndex ColumnListAdapter::adaptedIndex(int row, int column, const QModelIndex &parent) const
{
	return model()->index( row_.row(), row, row_.parent() );
}

int ColumnListAdapter::rowCount(const QModelIndex &parent) const
{
	return model()->columnCount( adaptedIndex( 0, 0, QModelIndex() ) );
}

void ColumnListAdapter::onParentDataChanged(const QModelIndex &topLeft, 
	const QModelIndex &bottomRight, const QVector<int> &roles)
{
	auto row = row_.row();
	auto parent = row_.parent();

	if (topLeft.row() <= row &&
		bottomRight.row() >= row &&
		topLeft.parent() == parent &&
		bottomRight.parent() == parent)
	{
		emit dataChanged( 
			createIndex(topLeft.column(), 0, topLeft.internalPointer()), 
			createIndex(bottomRight.column(), 0, bottomRight.internalPointer()), 
			roles );
	}
}
} // end namespace wgt
