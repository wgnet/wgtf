#include "i_list_adapter.hpp"

#include <cassert>

namespace wgt
{
IListAdapter::~IListAdapter()
{
}

void IListAdapter::reset()
{
	cachedAdaptedIndices_.clear();
}

void IListAdapter::connect()
{
	auto model = this->model();
	if (model == nullptr)
	{
		return;
	}

	connections_ += QObject::connect(model, &QAbstractItemModel::dataChanged, this, &IListAdapter::onParentDataChanged);
	connections_ += QObject::connect(model, &QAbstractItemModel::layoutAboutToBeChanged, this,
	                                 &IListAdapter::onParentLayoutAboutToBeChanged);
	connections_ +=
	QObject::connect(model, &QAbstractItemModel::layoutChanged, this, &IListAdapter::onParentLayoutChanged);
	connections_ += QObject::connect(model, &QAbstractItemModel::rowsAboutToBeInserted, this,
	                                 &IListAdapter::onParentRowsAboutToBeInserted);
	connections_ +=
	QObject::connect(model, &QAbstractItemModel::rowsInserted, this, &IListAdapter::onParentRowsInserted);
	connections_ += QObject::connect(model, &QAbstractItemModel::rowsAboutToBeRemoved, this,
	                                 &IListAdapter::onParentRowsAboutToBeRemoved);
	connections_ += QObject::connect(model, &QAbstractItemModel::rowsRemoved, this, &IListAdapter::onParentRowsRemoved);
	connections_ +=
	QObject::connect(model, &QAbstractItemModel::rowsAboutToBeMoved, this, &IListAdapter::onParentRowsAboutToBeMoved);
	connections_ += QObject::connect(model, &QAbstractItemModel::rowsMoved, this, &IListAdapter::onParentRowsMoved);
	connections_ += QObject::connect(model, &QAbstractItemModel::columnsAboutToBeInserted, this,
	                                 &IListAdapter::onParentColumnsAboutToBeInserted);
	connections_ +=
	QObject::connect(model, &QAbstractItemModel::columnsInserted, this, &IListAdapter::onParentColumnsInserted);
	connections_ += QObject::connect(model, &QAbstractItemModel::columnsAboutToBeRemoved, this,
	                                 &IListAdapter::onParentColumnsAboutToBeRemoved);
	connections_ +=
	QObject::connect(model, &QAbstractItemModel::columnsRemoved, this, &IListAdapter::onParentColumnsRemoved);
	connections_ += QObject::connect(model, &QAbstractItemModel::columnsAboutToBeMoved, this,
	                                 &IListAdapter::onParentColumnsAboutToBeMoved);
	connections_ +=
	QObject::connect(model, &QAbstractItemModel::columnsMoved, this, &IListAdapter::onParentColumnsMoved);
}

void IListAdapter::disconnect()
{
	connections_.reset();
}

QHash<int, QByteArray> IListAdapter::roleNames() const
{
	auto model = this->model();
	if (model == nullptr)
	{
		return QHash<int, QByteArray>();
	}

	return model->roleNames();
}

QModelIndex IListAdapter::index(int row, int column, const QModelIndex& parent) const
{
	auto cachedIndex = cachedAdaptedIndex(row, column, parent);
	return createIndex(row, column, cachedIndex.internalPointer());
}

QVariant IListAdapter::data(const QModelIndex& index, int role) const
{
	auto model = this->model();
	if (model == nullptr)
	{
		return QVariant(QVariant::Invalid);
	}

	auto cachedIndex = cachedAdaptedIndex(index.row(), index.column(), index.parent());
	if (!cachedIndex.isValid())
	{
		return QVariant(QVariant::Invalid);
	}
	return model->data(cachedIndex, role);
}

bool IListAdapter::setData(const QModelIndex& index, const QVariant& value, int role)
{
	auto model = this->model();
	if (model == nullptr)
	{
		return false;
	}

	auto cachedIndex = cachedAdaptedIndex(index.row(), index.column(), index.parent());
	if (!cachedIndex.isValid())
	{
		return false;
	}
	return model->setData(cachedIndex, value, role);
}

QModelIndex IListAdapter::cachedAdaptedIndex(int row, int column, const QModelIndex& parent) const
{
	assert(column == 0 && !parent.isValid());
	auto it = cachedAdaptedIndices_.find(row);
	if (it != cachedAdaptedIndices_.end())
	{
		return it->second;
	}

	auto index = adaptedIndex(row, column, parent);
	cachedAdaptedIndices_[row] = index;
	return index;
}
} // end namespace wgt
