#include "wg_sub_proxy.hpp"

#include "core_common/assert.hpp"
#include "core_qt_common/models/wgt_item_model_base.hpp"
#include "core_variant/variant.hpp"
#include <functional>


namespace wgt
{
struct WGSubProxy::Impl
	: public WgtItemModelBase
{
	Impl( WGSubProxy & subProxy )
		: subProxy_( subProxy )
	{
	}
	QModelIndex proxy_to_source(const QModelIndex& proxyIndex, QAbstractItemModel& sourceModel) const
	{
		if (!proxyIndex.isValid())
		{
			return sourceParent_;
		}

		// See ExtendedModel::sourceIndex for an explanation of the following code
		QModelIndex(QAbstractItemModel::*createIndexFunc)(int, int, void*) const = &WGSubProxy::createIndex;
		using namespace std::placeholders;
		auto createIndex = std::bind(createIndexFunc, &sourceModel, _1, _2, _3);
		return createIndex(proxyIndex.row(), proxyIndex.column(), proxyIndex.internalPointer());
	}

	QModelIndex source_to_proxy(const QModelIndex& sourceIndex, const WGSubProxy& proxyModel)
	{
		if (!sourceIndex.isValid() || sourceIndex == sourceParent_)
		{
			return QModelIndex();
		}

		TF_ASSERT(sourceIndex.isValid());
		return proxyModel.createIndex(sourceIndex.row(), sourceIndex.column(), sourceIndex.internalPointer());
	}

	bool isMapped(const QModelIndex& source_parent)
	{
		if (source_parent == sourceParent_)
		{
			return true;
		}

		if (!source_parent.isValid())
		{
			return false;
		}
		
		return isMapped(source_parent.parent());
	}


	QAbstractItemModel * getSourceModel() const override
	{
		return subProxy_.sourceModel();
	}

	
	QModelIndex getSourceIndex(const QModelIndex & index) const override
	{
		return proxy_to_source(index, *getSourceModel());
	}

	QtConnectionHolder connections_;
	QPersistentModelIndex sourceParent_;
	WGSubProxy & subProxy_;
};

WGSubProxy::WGSubProxy()
	: WGTInterfaceProvider( this )
	, impl_(new Impl( *this ))
{
	registerInterface(*impl_);
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
	auto source_index = source->index(row, column, source_parent);
	return mapFromSource(source_index);
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

	impl_->connections_.reset();
	impl_->sourceParent_ = sourceParent;
	sourceParentChanged();

	auto sourceModel = impl_->sourceParent_.model();
	QAbstractProxyModel::setSourceModel(const_cast<QAbstractItemModel*>(sourceModel));

	if (sourceModel != nullptr)
	{
		impl_->connections_ += QObject::connect(sourceModel, &QAbstractItemModel::modelAboutToBeReset, [this]() {
			setSourceParent(QModelIndex());
		});

		impl_->connections_ +=
		QObject::connect(sourceModel, &QAbstractItemModel::dataChanged,
		                 [this](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles) {
							 auto parent = topLeft.parent();
							 if (parent != bottomRight.parent())
							 {
								 return;
							 }
							 
							 if (!impl_->isMapped(parent))
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