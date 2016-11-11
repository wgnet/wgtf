#include "wg_column_layout_proxy.hpp"
#include "qt_connection_holder.hpp"

#include <cassert>

namespace wgt
{
struct WGColumnLayoutProxy::Impl
{
	struct Mapping
	{
		QHash<QModelIndex, Mapping*>::iterator mapIter_;
	};

	~Impl()
	{
		clearMapping();
	}

	QModelIndex proxy_to_source_parent(const QModelIndex& proxyIndex, QAbstractItemModel& sourceModel) const
	{
		if (!proxyIndex.isValid() || proxyIndex.column() != 0)
		{
			return QModelIndex();
		}

		auto it = index_to_iterator(proxyIndex);
		return sourceModel.index(proxyIndex.row(), 0, it.key());
	}

	QModelIndex proxy_to_source(const QModelIndex& proxyIndex, QAbstractItemModel& sourceModel) const
	{
		if (!proxyIndex.isValid())
		{
			return QModelIndex();
		}

		if (proxyIndex.column() >= columnSequence_.size())
		{
			return QModelIndex();
		}

		auto it = index_to_iterator(proxyIndex);
		return sourceModel.index(proxyIndex.row(), columnSequence_[proxyIndex.column()], it.key());
	}

	QModelIndex source_to_proxy_parent(const QModelIndex& sourceIndex, const WGColumnLayoutProxy& proxyModel)
	{
		if (!sourceIndex.isValid())
		{
			return QModelIndex();
		}

		auto source_parent = sourceIndex.parent();
		auto it = createMapping(source_parent);
		return proxyModel.createIndex(sourceIndex.row(), 0, *it);
	}

	QModelIndexList source_to_proxies(const QModelIndex& sourceIndex, const WGColumnLayoutProxy& proxyModel)
	{
		if (!sourceIndex.isValid())
		{
			return QModelIndexList();
		}

		auto source_parent = sourceIndex.parent();
		auto it = createMapping(source_parent);

		QModelIndexList proxies;
		for (int column = 0; column < columnSequence_.size(); ++column)
		{
			if (columnSequence_[column] == sourceIndex.column())
			{
				proxies.append(proxyModel.createIndex(sourceIndex.row(), column, *it));
			}
		}
		return proxies;
	}

	QHash<QModelIndex, Mapping*>::const_iterator index_to_iterator(const QModelIndex& proxy_index) const
	{
		auto* p = proxy_index.internalPointer();
		assert(p);
		auto it = static_cast<const Mapping*>(p)->mapIter_;
		assert(it != sourceIndexMapping_.end());
		assert(it.value());
		return it;
	}

	QHash<QModelIndex, Mapping*>::iterator createMapping(const QModelIndex& source_parent)
	{
		auto it = sourceIndexMapping_.find(source_parent);
		if (it != sourceIndexMapping_.constEnd()) // was mapped already
		{
			return it;
		}

		auto m = new Mapping;
		it = sourceIndexMapping_.insert(source_parent, m);
		m->mapIter_ = it;
		return it;
	}

	void removeMapping(const QModelIndex& source_parent, const WGColumnLayoutProxy& proxyModel)
	{
		if (!source_parent.isValid())
		{
			clearMapping();
			return;
		}

		auto it = sourceIndexMapping_.find(source_parent);
		if (it == sourceIndexMapping_.constEnd()) // not mapped
		{
			return;
		}

		auto source = source_parent.model();
		assert(source != nullptr);
		auto rowCount = source->rowCount(source_parent);
		for (auto row = 0; row < rowCount; ++row)
		{
			auto source_row = proxyModel.createIndex(row, 0, *it);
			removeMapping(source_row, proxyModel);
		}

		delete *it;
		sourceIndexMapping_.erase(it);
	}

	void clearMapping()
	{
		qDeleteAll(sourceIndexMapping_);
		sourceIndexMapping_.clear();
	}

	QList<int> columnSequence_;
	QtConnectionHolder connections_;
	QHash<QModelIndex, Mapping*> sourceIndexMapping_;
};

WGColumnLayoutProxy::WGColumnLayoutProxy() : impl_(new Impl())
{
}

WGColumnLayoutProxy::~WGColumnLayoutProxy()
{
}

void WGColumnLayoutProxy::setSourceModel(QAbstractItemModel* sourceModel)
{
	beginResetModel();
	impl_->clearMapping();
	impl_->connections_.reset();
	if (sourceModel != nullptr)
	{
		impl_->connections_ += QObject::connect(sourceModel, &QAbstractItemModel::modelAboutToBeReset, this,
		                                        &WGColumnLayoutProxy::onSourceModelAboutToBeReset);
		impl_->connections_ +=
		QObject::connect(sourceModel, &QAbstractItemModel::modelReset, this, &WGColumnLayoutProxy::onSourceModelReset);
		impl_->connections_ += QObject::connect(sourceModel, &QAbstractItemModel::dataChanged, this,
		                                        &WGColumnLayoutProxy::onSourceDataChanged);
		impl_->connections_ += QObject::connect(sourceModel, &QAbstractItemModel::headerDataChanged, this,
		                                        &WGColumnLayoutProxy::onSourceHeaderDataChanged);
		impl_->connections_ += QObject::connect(sourceModel, &QAbstractItemModel::layoutAboutToBeChanged, this,
		                                        &WGColumnLayoutProxy::onSourceLayoutAboutToBeChanged);
		impl_->connections_ += QObject::connect(sourceModel, &QAbstractItemModel::layoutChanged, this,
		                                        &WGColumnLayoutProxy::onSourceLayoutChanged);
		impl_->connections_ += QObject::connect(sourceModel, &QAbstractItemModel::rowsAboutToBeInserted, this,
		                                        &WGColumnLayoutProxy::onSourceRowsAboutToBeInserted);
		impl_->connections_ += QObject::connect(sourceModel, &QAbstractItemModel::rowsInserted, this,
		                                        &WGColumnLayoutProxy::onSourceRowsInserted);
		impl_->connections_ += QObject::connect(sourceModel, &QAbstractItemModel::rowsAboutToBeRemoved, this,
		                                        &WGColumnLayoutProxy::onSourceRowsAboutToBeRemoved);
		impl_->connections_ += QObject::connect(sourceModel, &QAbstractItemModel::rowsRemoved, this,
		                                        &WGColumnLayoutProxy::onSourceRowsRemoved);
		impl_->connections_ += QObject::connect(sourceModel, &QAbstractItemModel::rowsAboutToBeMoved, this,
		                                        &WGColumnLayoutProxy::onSourceRowsAboutToBeMoved);
		impl_->connections_ +=
		QObject::connect(sourceModel, &QAbstractItemModel::rowsMoved, this, &WGColumnLayoutProxy::onSourceRowsMoved);
		impl_->connections_ += QObject::connect(sourceModel, &QAbstractItemModel::columnsAboutToBeInserted, this,
		                                        &WGColumnLayoutProxy::onSourceColumnsAboutToBeInserted);
		impl_->connections_ += QObject::connect(sourceModel, &QAbstractItemModel::columnsInserted, this,
		                                        &WGColumnLayoutProxy::onSourceColumnsInserted);
		impl_->connections_ += QObject::connect(sourceModel, &QAbstractItemModel::columnsAboutToBeRemoved, this,
		                                        &WGColumnLayoutProxy::onSourceColumnsAboutToBeRemoved);
		impl_->connections_ += QObject::connect(sourceModel, &QAbstractItemModel::columnsRemoved, this,
		                                        &WGColumnLayoutProxy::onSourceColumnsRemoved);
		impl_->connections_ += QObject::connect(sourceModel, &QAbstractItemModel::columnsAboutToBeMoved, this,
		                                        &WGColumnLayoutProxy::onSourceColumnsAboutToBeMoved);
		impl_->connections_ += QObject::connect(sourceModel, &QAbstractItemModel::columnsMoved, this,
		                                        &WGColumnLayoutProxy::onSourceColumnsMoved);
	}
	QAbstractProxyModel::setSourceModel(sourceModel);
	endResetModel();
}

QModelIndex WGColumnLayoutProxy::mapToSource(const QModelIndex& proxyIndex) const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return QModelIndex();
	}

	return impl_->proxy_to_source(proxyIndex, *source);
}

QModelIndex WGColumnLayoutProxy::mapFromSource(const QModelIndex& sourceIndex) const
{
	auto proxies = mapAllFromSource(sourceIndex);
	if (proxies.empty())
	{
		return QModelIndex();
	}

	return proxies[0];
}

QModelIndex WGColumnLayoutProxy::mapToSourceParent(const QModelIndex& proxyIndex) const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return QModelIndex();
	}

	return impl_->proxy_to_source_parent(proxyIndex, *source);
}

QModelIndex WGColumnLayoutProxy::mapFromSourceParent(const QModelIndex& sourceIndex) const
{
	return impl_->source_to_proxy_parent(sourceIndex, *this);
}

QModelIndexList WGColumnLayoutProxy::mapAllFromSource(const QModelIndex& sourceIndex) const
{
	return impl_->source_to_proxies(sourceIndex, *this);
}

QModelIndex WGColumnLayoutProxy::index(int row, int column, const QModelIndex& parent) const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return QModelIndex();
	}

	if (column >= impl_->columnSequence_.size())
	{
		return QModelIndex();
	}

	auto source_parent = mapToSource(parent);
	auto it = impl_->createMapping(source_parent);
	return createIndex(row, column, *it);
}

QModelIndex WGColumnLayoutProxy::parent(const QModelIndex& child) const
{
	if (!child.isValid())
	{
		return QModelIndex();
	}

	auto source_parent = impl_->index_to_iterator(child).key();
	if (!source_parent.isValid())
	{
		return QModelIndex();
	}

	return impl_->source_to_proxy_parent(source_parent, *this);
}

int WGColumnLayoutProxy::rowCount(const QModelIndex& parent) const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return 0;
	}

	auto source_parent = mapToSourceParent(parent);
	if (parent.isValid() && !source_parent.isValid())
	{
		return 0;
	}

	return source->rowCount(source_parent);
}

int WGColumnLayoutProxy::columnCount(const QModelIndex& parent) const
{
	return impl_->columnSequence_.size();
}

bool WGColumnLayoutProxy::hasChildren(const QModelIndex& parent) const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return false;
	}

	auto source_parent = mapToSourceParent(parent);
	if (parent.isValid() && !source_parent.isValid())
	{
		return false;
	}

	return source->hasChildren(source_parent);
}

QVariant WGColumnLayoutProxy::data(const QModelIndex& index, int role) const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return QVariant();
	}

	auto source_index = mapToSource(index);
	if (!source_index.isValid())
	{
		return QVariant();
	}

	return source->data(source_index, role);
}

bool WGColumnLayoutProxy::setData(const QModelIndex& index, const QVariant& value, int role)
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return false;
	}

	auto source_index = mapToSource(index);
	if (!source_index.isValid())
	{
		return false;
	}

	return source->setData(source_index, value, role);
}

QVariant WGColumnLayoutProxy::headerData(int section, Qt::Orientation orientation, int role) const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return QVariant();
	}

	if (orientation == Qt::Vertical)
	{
		return source->headerData(section, orientation, role);
	}
	else
	{
		if (section >= impl_->columnSequence_.size())
		{
			return QVariant();
		}

		return source->headerData(impl_->columnSequence_[section], orientation, role);
	}
}

bool WGColumnLayoutProxy::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return false;
	}

	if (orientation == Qt::Vertical)
	{
		return source->setHeaderData(section, orientation, value, role);
	}
	else
	{
		if (section >= impl_->columnSequence_.size())
		{
			return false;
		}

		return source->setHeaderData(impl_->columnSequence_[section], orientation, value, role);
	}
}

QHash<int, QByteArray> WGColumnLayoutProxy::roleNames() const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return QHash<int, QByteArray>();
	}

	return source->roleNames();
}

QList<int> WGColumnLayoutProxy::getColumnSequence() const
{
	return impl_->columnSequence_;
}

void WGColumnLayoutProxy::setColumnSequence(const QList<int>& columnSequence)
{
	auto source = sourceModel();
	if (source != nullptr)
	{
		beginResetModel();
		impl_->clearMapping();
	}
	impl_->columnSequence_ = columnSequence;
	if (source != nullptr)
	{
		endResetModel();
	}
	emit columnSequenceChanged();
}

void WGColumnLayoutProxy::onSourceModelAboutToBeReset()
{
	beginResetModel();
}

void WGColumnLayoutProxy::onSourceModelReset()
{
	impl_->clearMapping();
	endResetModel();
}

void WGColumnLayoutProxy::onSourceDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight,
                                              const QVector<int>& roles)
{
	if (!topLeft.isValid())
	{
		emit dataChanged(QModelIndex(), QModelIndex(), roles);
		return;
	}

	assert(bottomRight.isValid());
	auto source_parent = topLeft.parent();
	auto it = impl_->createMapping(source_parent);

	int first = topLeft.column();
	int last = bottomRight.column();
	int proxy_first = -1;
	int proxy_last = -1;
	for (int column = 0; column < impl_->columnSequence_.size(); ++column)
	{
		auto source_column = impl_->columnSequence_[column];
		if (source_column >= first && source_column <= last)
		{
			if (proxy_first == -1)
			{
				proxy_first = column;
			}
			proxy_last = column;
			continue;
		}

		if (proxy_first != -1 && proxy_last != -1)
		{
			auto proxy_topLeft = createIndex(topLeft.row(), proxy_first, *it);
			auto proxy_bottomRight = createIndex(bottomRight.row(), proxy_last, *it);
			emit dataChanged(proxy_topLeft, proxy_bottomRight, roles);
			proxy_first = -1;
			proxy_last = -1;
		}
	}
	if (proxy_first != -1 && proxy_last != -1)
	{
		auto proxy_topLeft = createIndex(topLeft.row(), proxy_first, *it);
		auto proxy_bottomRight = createIndex(bottomRight.row(), proxy_last, *it);
		emit dataChanged(proxy_topLeft, proxy_bottomRight, roles);
	}
}

void WGColumnLayoutProxy::onSourceHeaderDataChanged(Qt::Orientation orientation, int first, int last)
{
	if (orientation == Qt::Vertical)
	{
		emit headerDataChanged(orientation, first, last);
		return;
	}

	int proxy_first = -1;
	int proxy_last = -1;
	for (int column = 0; column < impl_->columnSequence_.size(); ++column)
	{
		auto source_column = impl_->columnSequence_[column];
		if (source_column >= first && source_column <= last)
		{
			if (proxy_first == -1)
			{
				proxy_first = column;
			}
			proxy_last = column;
			continue;
		}

		if (proxy_first != -1 && proxy_last != -1)
		{
			emit headerDataChanged(orientation, proxy_first, proxy_last);
			proxy_first = -1;
			proxy_last = -1;
		}
	}
	if (proxy_first != -1 && proxy_last != -1)
	{
		emit headerDataChanged(orientation, proxy_first, proxy_last);
	}
}

void WGColumnLayoutProxy::onSourceLayoutAboutToBeChanged(const QList<QPersistentModelIndex>& parents,
                                                         QAbstractItemModel::LayoutChangeHint hint)
{
	QList<QPersistentModelIndex> proxy_parents;
	for (auto& parent : parents)
	{
		auto proxy_parent = mapFromSourceParent(parent);
		proxy_parents.append(proxy_parent);
	}
	emit layoutAboutToBeChanged(proxy_parents, hint);
}

void WGColumnLayoutProxy::onSourceLayoutChanged(const QList<QPersistentModelIndex>& parents,
                                                QAbstractItemModel::LayoutChangeHint hint)
{
	if (parents.empty())
	{
		impl_->clearMapping();
	}
	else
	{
		for (auto& parent : parents)
		{
			impl_->removeMapping(parent, *this);
		}
	}

	QList<QPersistentModelIndex> proxy_parents;
	for (auto& parent : parents)
	{
		auto proxy_parent = mapFromSourceParent(parent);
		proxy_parents.append(proxy_parent);
	}
	emit layoutChanged(proxy_parents, hint);
}

void WGColumnLayoutProxy::onSourceRowsAboutToBeInserted(const QModelIndex& parent, int first, int last)
{
	auto proxy_parent = mapFromSourceParent(parent);
	beginInsertRows(proxy_parent, first, last);
}

void WGColumnLayoutProxy::onSourceRowsInserted()
{
	endInsertRows();
}

void WGColumnLayoutProxy::onSourceRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
{
	auto proxy_parent = mapFromSourceParent(parent);
	beginRemoveRows(proxy_parent, first, last);
}

void WGColumnLayoutProxy::onSourceRowsRemoved()
{
	endRemoveRows();
}

void WGColumnLayoutProxy::onSourceRowsAboutToBeMoved(const QModelIndex& sourceParent, int sourceStart, int sourceEnd,
                                                     const QModelIndex& destinationParent, int destinationRow)
{
	auto proxy_sourceParent = mapFromSourceParent(sourceParent);
	auto proxy_destinationParent = mapFromSourceParent(destinationParent);
	beginMoveRows(proxy_sourceParent, sourceStart, sourceEnd, proxy_destinationParent, destinationRow);
}

void WGColumnLayoutProxy::onSourceRowsMoved(const QModelIndex& parent, int start, int end,
                                            const QModelIndex& destination, int row)
{
	endMoveRows();
}

void WGColumnLayoutProxy::onSourceColumnsAboutToBeInserted(const QModelIndex& parent, int first, int last)
{
	layoutAboutToBeChanged();
}

void WGColumnLayoutProxy::onSourceColumnsInserted()
{
	impl_->clearMapping();
	emit layoutChanged();
}

void WGColumnLayoutProxy::onSourceColumnsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
{
	layoutAboutToBeChanged();
}

void WGColumnLayoutProxy::onSourceColumnsRemoved()
{
	impl_->clearMapping();
	emit layoutChanged();
}

void WGColumnLayoutProxy::onSourceColumnsAboutToBeMoved(const QModelIndex& sourceParent, int sourceStart, int sourceEnd,
                                                        const QModelIndex& destinationParent, int destinationColumn)
{
	layoutAboutToBeChanged();
}

void WGColumnLayoutProxy::onSourceColumnsMoved(const QModelIndex& parent, int start, int end,
                                               const QModelIndex& destination, int column)
{
	impl_->clearMapping();
	emit layoutChanged();
}
}