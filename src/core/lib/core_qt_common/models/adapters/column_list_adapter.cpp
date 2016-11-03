#include "column_list_adapter.hpp"

#include <cassert>


namespace wgt
{
ColumnListAdapter::ColumnListAdapter(const QModelIndex& row)
    : model_(row.model())
    , row_(row)
{
	assert(row_.isValid());
	parent_ = row_.parent();
	connect();
}

ColumnListAdapter::~ColumnListAdapter()
{
	disconnect();
}

QAbstractItemModel* ColumnListAdapter::model() const
{
	return const_cast<QAbstractItemModel*>(model_);
}

QModelIndex ColumnListAdapter::adaptedIndex(int row, int column, const QModelIndex& parent) const
{
	return model()->index(row_.row(), row, parent_);
}

int ColumnListAdapter::rowCount(const QModelIndex& parent) const
{
	return model()->columnCount();
}

void ColumnListAdapter::onParentDataChanged(const QModelIndex& topLeft,
                                            const QModelIndex& bottomRight, const QVector<int>& roles)
{
	auto row = row_.row();

	if (topLeft.row() <= row &&
	    bottomRight.row() >= row &&
	    topLeft.parent() == parent_ &&
	    bottomRight.parent() == parent_)
	{
		emit dataChanged(
		createIndex(topLeft.column(), 0, topLeft.internalPointer()),
		createIndex(bottomRight.column(), 0, bottomRight.internalPointer()),
		roles);
	}
}

void ColumnListAdapter::onParentLayoutChanged(const QList<QPersistentModelIndex>& parents,
                                              ColumnListAdapter::QAbstractItemModel::LayoutChangeHint hint)
{
	if (parents.empty())
	{
		emit dataChanged(index(0), index(rowCount(QModelIndex())));
	}
}

void ColumnListAdapter::onParentColumnsAboutToBeInserted(const QModelIndex& parent, int first, int last)
{
	if (!parent.isValid())
	{
		beginInsertRows(QModelIndex(), first, last);
	}
}

void ColumnListAdapter::onParentColumnsInserted(const QModelIndex& parent, int first, int last)
{
	if (!parent.isValid())
	{
		this->reset();
		endInsertRows();
	}
}

void ColumnListAdapter::onParentColumnsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
{
	if (!parent.isValid())
	{
		beginRemoveRows(QModelIndex(), first, last);
	}
}

void ColumnListAdapter::onParentColumnsRemoved(const QModelIndex& parent, int first, int last)
{
	if (!parent.isValid())
	{
		this->reset();
		endRemoveRows();
	}
}

void ColumnListAdapter::onParentColumnsAboutToBeMoved(const QModelIndex& sourceParent,
                                                      int sourceFirst,
                                                      int sourceLast,
                                                      const QModelIndex& destinationParent,
                                                      int destinationColumn)
{
	if (!sourceParent.isValid() && !destinationParent.isValid())
	{
		beginMoveRows(QModelIndex(), sourceFirst, sourceLast, QModelIndex(), destinationColumn);
	}
}

void ColumnListAdapter::onParentColumnsMoved(const QModelIndex& sourceParent,
                                             int sourceFirst,
                                             int sourceLast,
                                             const QModelIndex& destinationParent,
                                             int destinationColumn)
    {
	    if (!sourceParent.isValid() && !destinationParent.isValid())
	    {
		    this->reset();
		    endMoveRows();
	    }
    }
} // end namespace wgt
