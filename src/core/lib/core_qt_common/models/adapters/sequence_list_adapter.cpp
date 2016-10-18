#include "sequence_list_adapter.hpp"
#include <cassert>

namespace wgt
{
SequenceListAdapter::SequenceListAdapter()
    : model_(nullptr)
{
}

SequenceListAdapter::~SequenceListAdapter()
{
}

QAbstractItemModel* SequenceListAdapter::model() const
{
	return model_;
}

QModelIndex SequenceListAdapter::adaptedIndex(int row, int column, const QModelIndex& parent) const
    {
	    if (model() == nullptr)
	    {
		    return QModelIndex();
	    }

	    if (sequence_.empty())
	    {
		    return model()->index(row, column, parent);
	    }

	    if (row < sequence_.size())
	    {
		    return model()->index(sequence_[row], column, parent);
	    }
	    return QModelIndex();
	}

    int SequenceListAdapter::rowCount(const QModelIndex& parent) const
    {
	    if (model() == nullptr)
	    {
		    return 0;
	    }

	    if (sequence_.empty())
	    {
		    return model()->rowCount(parent);
	    }

	    return sequence_.size();
    }

    QVariant SequenceListAdapter::getModel() const
    {
	    return QVariant::fromValue<QAbstractItemModel*>(model_);
    }

    void SequenceListAdapter::setModel(const QVariant& model)
    {
	    auto m = model.value<QAbstractItemModel*>();
	    if (model_ == m)
	    {
		    return;
	    }

	    beginResetModel();
	    disconnect();
	    reset();
	    model_ = m;
	    connect();
	    endResetModel();
    }

    QList<int> SequenceListAdapter::getSequence()
    {
	    return sequence_;
    }

    void SequenceListAdapter::setSequence(const QList<int>& sequence)
    {
	    layoutAboutToBeChanged();
	    reset();
	    sequence_ = sequence;
	    layoutChanged();
    }

    void SequenceListAdapter::onParentDataChanged(const QModelIndex& topLeft,
                                                  const QModelIndex& bottomRight, const QVector<int>& roles)
    {
	    if (sequence_.empty())
	    {
		    emit dataChanged(
		    createIndex(topLeft.row(), topLeft.column(), topLeft.internalPointer()),
		    createIndex(bottomRight.row(), bottomRight.column(), bottomRight.internalPointer()),
		    roles);
		    return;
	    }

	    for (int i = topLeft.row(); i <= bottomRight.row(); ++i)
	    {
		    int index = 0;
		    for (;;)
		    {
			    index = sequence_.indexOf(i, index);
			    if (index < 0)
			    {
				    break;
			    }

			    emit dataChanged(
			    createIndex(index, topLeft.column()),
			    createIndex(index, bottomRight.column()),
			    roles);
			    ++index;
		    }
	    }
    }

    void SequenceListAdapter::onParentRowsAboutToBeInserted(const QModelIndex& parent, int first, int last)
    {
	    if (!sequence_.empty())
	    {
		    assert(false && "Not supported");
	    }

	    auto index = parent.isValid() ? createIndex(parent.row(), parent.column(), parent.internalPointer()) : parent;
	    beginInsertRows(index, first, last);
    }

    void SequenceListAdapter::onParentRowsInserted(const QModelIndex& parent, int first, int last)
    {
	    if (!sequence_.empty())
	    {
		    assert(false && "Not supported");
	    }

	    this->reset();
	    endInsertRows();
    }

    void SequenceListAdapter::onParentRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
    {
	    if (!sequence_.empty())
	    {
		    assert(false && "Not supported");
	    }

	    auto index = parent.isValid() ? createIndex(parent.row(), parent.column(), parent.internalPointer()) : parent;
	    beginRemoveRows(index, first, last);
    }

    void SequenceListAdapter::onParentRowsRemoved(const QModelIndex& parent, int first, int last)
    {
	    if (!sequence_.empty())
	    {
		    assert(false && "Not supported");
	    }

	    this->reset();
	    endRemoveRows();
    }

    void SequenceListAdapter::onParentRowsAboutToBeMoved(const QModelIndex& sourceParent,
                                                         int sourceFirst,
                                                         int sourceLast,
                                                         const QModelIndex& destinationParent,
                                                         int destinationRow) /* override */
    {
	    if (!sequence_.empty())
	    {
		    assert(false && "Not supported");
	    }

	    auto sourceIndex = sourceParent.isValid() ? createIndex(sourceParent.row(), sourceParent.column(), sourceParent.internalPointer()) : sourceParent;
	    auto destinationIndex = destinationParent.isValid() ? createIndex(destinationParent.row(), destinationParent.column(), destinationParent.internalPointer()) : destinationParent;
	    beginMoveRows(sourceIndex,
	                  sourceFirst,
	                  sourceLast,
	                  destinationIndex,
	                  destinationRow);
    }

    void SequenceListAdapter::onParentRowsMoved(const QModelIndex& sourceParent,
                                                int sourceFirst,
                                                int sourceLast,
                                                const QModelIndex& destinationParent,
                                                int destinationRow) /* override */
    {
	    if (!sequence_.empty())
	    {
		    assert(false && "Not supported");
	    }

	    this->reset();
	    endMoveRows();
    }

} // end namespace wgt
