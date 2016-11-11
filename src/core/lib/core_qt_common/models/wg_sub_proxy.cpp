#include "wg_sub_proxy.hpp"
#include <assert.h>

namespace wgt
{
struct WGSubProxy::Impl
{
	struct Mapping
	{
		QHash<QModelIndex, Mapping*>::iterator mapIter_;
	};

	~Impl()
	{
		clearMapping();
	}

	QModelIndex proxy_to_source(const QModelIndex& proxyIndex, QAbstractItemModel& sourceModel) const
	{
		if (!proxyIndex.isValid())
		{
			return sourceParent_;
		}

		auto it = index_to_iterator(proxyIndex);
		return sourceModel.index(proxyIndex.row(), proxyIndex.column(), it.key());
	}

	QModelIndex source_to_proxy(const QModelIndex& sourceIndex, const WGSubProxy& proxyModel)
	{
		if (!sourceIndex.isValid() || sourceIndex == sourceParent_)
		{
			return QModelIndex();
		}

		auto source_parent = sourceIndex.parent();
		auto it = createMapping(source_parent);
		return proxyModel.createIndex(sourceIndex.row(), sourceIndex.column(), *it);
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

	bool isMapped(const QModelIndex& source_parent)
	{
		if (!source_parent.isValid())
		{
			return false;
		}

		if (source_parent == sourceParent_)
		{
			return true;
		}

		auto it = sourceIndexMapping_.find(source_parent);
		return it != sourceIndexMapping_.constEnd();
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

	void clearMapping()
	{
		qDeleteAll(sourceIndexMapping_);
		sourceIndexMapping_.clear();
	}

	QtConnectionHolder connections_;
	QPersistentModelIndex sourceParent_;
	QHash<QModelIndex, Mapping*> sourceIndexMapping_;
};

WGSubProxy::WGSubProxy() : impl_(new Impl())
{
}

WGSubProxy::~WGSubProxy()
{
}

void WGSubProxy::setSourceModel(QAbstractItemModel* model)
{
	auto source = sourceModel();
	if (model == source)
	{
		return;
	}

	setSourceParent(QModelIndex());
}

QModelIndex WGSubProxy::mapToSource(const QModelIndex& proxyIndex) const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return QModelIndex();
	}

	return impl_->proxy_to_source(proxyIndex, *source);
}

QModelIndex WGSubProxy::mapFromSource(const QModelIndex& sourceIndex) const
{
	return impl_->source_to_proxy(sourceIndex, *this);
}

QModelIndex WGSubProxy::index(int row, int column, const QModelIndex& parent) const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return QModelIndex();
	}

	auto source_parent = mapToSource(parent);
	auto it = impl_->createMapping(source_parent);
	return createIndex(row, column, *it);
}

QModelIndex WGSubProxy::parent(const QModelIndex& child) const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return QModelIndex();
	}

	auto sourceParent = source->parent(mapToSource(child));
	return mapFromSource(sourceParent);
}

int WGSubProxy::rowCount(const QModelIndex& parent) const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return 0;
	}

	return source->rowCount(mapToSource(parent));
}

int WGSubProxy::columnCount(const QModelIndex& parent) const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return 0;
	}

	return source->columnCount(mapToSource(parent));
}

bool WGSubProxy::hasChildren(const QModelIndex& parent) const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return false;
	}

	return source->hasChildren(mapToSource(parent));
}

QVariant WGSubProxy::headerData(int section, Qt::Orientation orientation, int role) const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return QVariant();
	}

	return source->headerData(section, orientation, role);
}

bool WGSubProxy::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return false;
	}

	return source->setHeaderData(section, orientation, value, role);
}

QHash<int, QByteArray> WGSubProxy::roleNames() const
{
	auto source = sourceModel();
	if (source == nullptr)
	{
		return QHash<int, QByteArray>();
	}

	return source->roleNames();
}

QModelIndex WGSubProxy::getSourceParent() const
{
	return impl_->sourceParent_;
}

void WGSubProxy::setSourceParent(const QModelIndex& sourceParent)
{
	if (impl_->sourceParent_ == sourceParent)
	{
		return;
	}

	beginResetModel();

	impl_->clearMapping();

	impl_->connections_.reset();

	impl_->sourceParent_ = sourceParent;
	sourceParentChanged();

	auto sourceModel = impl_->sourceParent_.model();
	QAbstractProxyModel::setSourceModel(const_cast<QAbstractItemModel*>(sourceModel));

	if (sourceModel != nullptr)
	{
		impl_->connections_ += QObject::connect(sourceModel, &QAbstractItemModel::modelAboutToBeReset, [this]() {
			beginResetModel();
			impl_->clearMapping();
		});
		impl_->connections_ +=
		QObject::connect(sourceModel, &QAbstractItemModel::modelReset, [this]() { endResetModel(); });

		impl_->connections_ +=
		QObject::connect(sourceModel, &QAbstractItemModel::dataChanged,
		                 [this](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles) {
			                 if (!impl_->isMapped(topLeft) || !impl_->isMapped(bottomRight))
			                 {
				                 return;
			                 }

			                 auto proxyTopLeft = mapFromSource(topLeft);
			                 auto proxyBottomRight = mapFromSource(bottomRight);
			                 dataChanged(proxyTopLeft, proxyBottomRight, roles);
			             });

		impl_->connections_ += QObject::connect(sourceModel, &QAbstractItemModel::rowsAboutToBeInserted,
		                                        [this](const QModelIndex& parent, int first, int last) {
			                                        if (!impl_->isMapped(parent))
			                                        {
				                                        return;
			                                        }

			                                        auto proxy_parent = mapFromSource(parent);
			                                        beginInsertRows(proxy_parent, first, last);
			                                    });
		impl_->connections_ += QObject::connect(sourceModel, &QAbstractItemModel::rowsInserted,
		                                        [this](const QModelIndex& parent, int first, int last) {
			                                        if (!impl_->isMapped(parent))
			                                        {
				                                        return;
			                                        }

			                                        endInsertRows();
			                                    });
		impl_->connections_ += QObject::connect(sourceModel, &QAbstractItemModel::rowsAboutToBeRemoved,
		                                        [this](const QModelIndex& parent, int first, int last) {
			                                        if (!impl_->isMapped(parent))
			                                        {
				                                        return;
			                                        }

			                                        auto proxy_parent = mapFromSource(parent);
			                                        beginRemoveRows(proxy_parent, first, last);
			                                    });
		impl_->connections_ += QObject::connect(sourceModel, &QAbstractItemModel::rowsRemoved,
		                                        [this](const QModelIndex& parent, int first, int last) {
			                                        if (!impl_->isMapped(parent))
			                                        {
				                                        return;
			                                        }

			                                        endRemoveRows();
			                                    });

		impl_->connections_ += QObject::connect(sourceModel, &QAbstractItemModel::columnsAboutToBeInserted,
		                                        [this](const QModelIndex& parent, int first, int last) {
			                                        if (!impl_->isMapped(parent))
			                                        {
				                                        return;
			                                        }

			                                        auto proxy_parent = mapFromSource(parent);
			                                        beginInsertColumns(proxy_parent, first, last);
			                                    });
		impl_->connections_ += QObject::connect(sourceModel, &QAbstractItemModel::columnsInserted,
		                                        [this](const QModelIndex& parent, int first, int last) {
			                                        if (!impl_->isMapped(parent))
			                                        {
				                                        return;
			                                        }

			                                        endInsertColumns();
			                                    });
		impl_->connections_ += QObject::connect(sourceModel, &QAbstractItemModel::columnsAboutToBeRemoved,
		                                        [this](const QModelIndex& parent, int first, int last) {
			                                        if (!impl_->isMapped(parent))
			                                        {
				                                        return;
			                                        }

			                                        auto proxy_parent = mapFromSource(parent);
			                                        beginRemoveColumns(proxy_parent, first, last);
			                                    });
		impl_->connections_ += QObject::connect(sourceModel, &QAbstractItemModel::columnsRemoved,
		                                        [this](const QModelIndex& parent, int first, int last) {
			                                        if (!impl_->isMapped(parent))
			                                        {
				                                        return;
			                                        }

			                                        endRemoveColumns();
			                                    });
	}

	endResetModel();
}
}