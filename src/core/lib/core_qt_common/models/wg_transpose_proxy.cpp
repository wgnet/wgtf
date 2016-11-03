#include "wg_transpose_proxy.hpp"
#include <assert.h>

namespace wgt
{
void WGTransposeProxy::setSourceModel(QAbstractItemModel* sourceModel)
{
	beginResetModel();
	connections_.reset();
	QAbstractProxyModel::setSourceModel(sourceModel);
	if (sourceModel != nullptr)
	{
		connections_ += QObject::connect(sourceModel, &QAbstractItemModel::modelAboutToBeReset, [this]() { beginResetModel(); });
		connections_ += QObject::connect(sourceModel, &QAbstractItemModel::modelReset, [this]() { endResetModel(); });
		connections_ += QObject::connect(sourceModel, &QAbstractItemModel::dataChanged,
		                                 [this](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
		                                 {
			                                 auto proxyTopLeft = topLeft.isValid() ? index(topLeft.column(), topLeft.row()) : QModelIndex();
			                                 auto proxyBottomRight = bottomRight.isValid() ? index(bottomRight.column(), bottomRight.row()) : QModelIndex();
			                                 dataChanged(proxyTopLeft, proxyBottomRight, roles);
			                             });
		connections_ += QObject::connect(sourceModel, &QAbstractItemModel::rowsAboutToBeInserted,
		                                 [this](const QModelIndex& parent, int first, int last)
		                                 {
			                                 assert(!parent.isValid());
			                                 beginInsertColumns(QModelIndex(), first, last);
			                             });
		connections_ += QObject::connect(sourceModel, &QAbstractItemModel::rowsInserted, [this]() { endInsertColumns(); });
		connections_ += QObject::connect(sourceModel, &QAbstractItemModel::rowsAboutToBeRemoved,
		                                 [this](const QModelIndex& parent, int first, int last)
		                                 {
			                                 assert(!parent.isValid());
			                                 beginRemoveColumns(QModelIndex(), first, last);
			                             });
		connections_ += QObject::connect(sourceModel, &QAbstractItemModel::rowsRemoved, [this]() { endRemoveColumns(); });
		connections_ += QObject::connect(sourceModel, &QAbstractItemModel::columnsAboutToBeInserted,
		                                 [this](const QModelIndex& parent, int first, int last)
		                                 {
			                                 assert(!parent.isValid());
			                                 beginInsertRows(QModelIndex(), first, last);
			                             });
		connections_ += QObject::connect(sourceModel, &QAbstractItemModel::columnsInserted, [this]() { endInsertRows(); });
		connections_ += QObject::connect(sourceModel, &QAbstractItemModel::columnsAboutToBeRemoved,
		                                 [this](const QModelIndex& parent, int first, int last)
		                                 {
			                                 assert(!parent.isValid());
			                                 beginRemoveRows(QModelIndex(), first, last);
			                             });
		connections_ += QObject::connect(sourceModel, &QAbstractItemModel::columnsRemoved, [this]() { endRemoveRows(); });
	}
	endResetModel();
}

QModelIndex WGTransposeProxy::mapToSource(const QModelIndex& proxyIndex) const
{
	if (proxyIndex.parent().isValid())
	{
		return QModelIndex();
	}

	return sourceModel()->index(proxyIndex.column(), proxyIndex.row());
}

QModelIndex WGTransposeProxy::mapFromSource(const QModelIndex& sourceIndex) const
{
	return createIndex(sourceIndex.column(), sourceIndex.row(), sourceIndex.internalPointer());
}

QModelIndex WGTransposeProxy::index(int row, int column, const QModelIndex& parent) const
{
	auto source = sourceModel();
	if (source == nullptr || parent.isValid())
	{
		return QModelIndex();
	}

	return mapFromSource(source->index(column, row));
}

QModelIndex WGTransposeProxy::parent(const QModelIndex& child) const
{
	return QModelIndex();
}

int WGTransposeProxy::rowCount(const QModelIndex& parent) const
{
	auto source = sourceModel();
	if (source == nullptr || parent.isValid())
	{
		return 0;
	}

	return source->columnCount();
}

int WGTransposeProxy::columnCount(const QModelIndex& parent) const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return 0;
	}

	return source->rowCount();
}

bool WGTransposeProxy::hasChildren(const QModelIndex& parent) const
{
	return rowCount(parent);
}

QVariant WGTransposeProxy::headerData(int section, Qt::Orientation orientation, int role) const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return QVariant();
	}

	return source->headerData(section, orientation == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal, role);
}

bool WGTransposeProxy::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return false;
	}

	return source->setHeaderData(section, orientation == Qt::Horizontal ? Qt::Vertical : Qt::Horizontal, value, role);
}

QHash<int, QByteArray> WGTransposeProxy::roleNames() const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return QHash<int, QByteArray>();
	}

	return source->roleNames();
}
}