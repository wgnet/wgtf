#include "child_list_adapter.hpp"
#include <QPersistentModelIndex>

namespace wgt
{
ChildListAdapter::ChildListAdapter(const QModelIndex& parent, bool connect) : parent_(parent), connect_(connect)
{
	if (connect_)
	{
		this->connect();
	}
	else
	{
		connections_ +=
		QObject::connect(this->model(), &QAbstractItemModel::dataChanged, this, &ChildListAdapter::onParentDataChanged);
		connections_ +=
		QObject::connect(this->model(), &QAbstractItemModel::rowsAboutToBeMoved, this, &ChildListAdapter::onParentRowsAboutToBeMoved);
		connections_ +=
		QObject::connect(this->model(), &QAbstractItemModel::rowsMoved, this, &ChildListAdapter::onParentRowsMoved);
	}
}

ChildListAdapter::~ChildListAdapter()
{
	this->disconnect();
}

QAbstractItemModel* ChildListAdapter::model() const
{
	return const_cast<QAbstractItemModel*>(parent_.model());
}

QModelIndex ChildListAdapter::adaptedIndex(int row, int column, const QModelIndex& parent) const
{
	auto m = model();

	if (m == nullptr)
	{
		return QModelIndex();
	}

	return m->index(row, column, parent_);
}

int ChildListAdapter::rowCount(const QModelIndex& parent) const
{
	auto m = model();

	if (m == nullptr)
	{
		return 0;
	}

	return m->rowCount(parent_);
}

void ChildListAdapter::onParentDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight,
                                           const QVector<int>& roles)
{
	if (topLeft.parent() == parent_ && bottomRight.parent() == parent_)
	{
		emit dataChanged(createIndex(topLeft.row(), topLeft.column()),
		                 createIndex(bottomRight.row(), bottomRight.column()), roles);
	}
}

void ChildListAdapter::onParentLayoutAboutToBeChanged(const QList<QPersistentModelIndex>& parents,
                                                      QAbstractItemModel::LayoutChangeHint hint)
{
	auto resetLayout = parents.empty();
	if (!resetLayout)
	{
		auto item = static_cast<QModelIndex>(parent_).internalPointer();
		for (auto it = parents.cbegin(); it != parents.cend(); ++it)
		{
			if (static_cast<QModelIndex>(*it).internalPointer() == item)
			{
				resetLayout = true;
				break;
			}
		}
	}
	if (resetLayout)
	{
		emit layoutAboutToBeChanged();
	}
}

void ChildListAdapter::onParentLayoutChanged(const QList<QPersistentModelIndex>& parents,
                                             QAbstractItemModel::LayoutChangeHint hint)
{
	auto resetLayout = parents.empty();
	if (!resetLayout)
	{
		auto item = static_cast<QModelIndex>(parent_).internalPointer();
		for (auto it = parents.cbegin(); it != parents.cend(); ++it)
		{
			if (static_cast<QModelIndex>(*it).internalPointer() == item)
			{
				resetLayout = true;
				break;
			}
		}
	}
	if (resetLayout)
	{
		reset();
		emit layoutChanged();
	}
}

void ChildListAdapter::onParentRowsAboutToBeInserted(const QModelIndex& parent, int first, int last)
{
	if (parent != parent_)
	{
		return;
	}

	beginInsertRows(QModelIndex(), first, last);
}

void ChildListAdapter::onParentRowsInserted(const QModelIndex& parent, int first, int last)
{
	if (parent != parent_)
	{
		return;
	}

	reset();
	endInsertRows();
}

void ChildListAdapter::onParentRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
{
	if (parent != parent_)
	{
		return;
	}

	removedParent_ = parent_;
	beginRemoveRows(QModelIndex(), first, last);
}

void ChildListAdapter::onParentRowsRemoved(const QModelIndex& parent, int first, int last)
{
	if (removedParent_ == QModelIndex())
	{
		return;
	}

	removedParent_ = QModelIndex();
	reset();
	endRemoveRows();
}

void ChildListAdapter::onParentRowsAboutToBeMoved(const QModelIndex& sourceParent, int sourceFirst, int sourceLast,
                                                  const QModelIndex& destinationParent,
                                                  int destinationRow) /* override */
{
	if (sourceParent != parent_ || destinationParent != parent_)
	{
		return;
	}

	beginMoveRows(QModelIndex(), sourceFirst, sourceLast, QModelIndex(), destinationRow);
}

void ChildListAdapter::onParentRowsMoved(const QModelIndex& sourceParent, int sourceFirst, int sourceLast,
                                         const QModelIndex& destinationParent, int destinationRow) /* override */
{
	if (sourceParent != parent_ || destinationParent != parent_)
	{
		return;
	}

	this->reset();
	endMoveRows();
}
} // end namespace wgt
