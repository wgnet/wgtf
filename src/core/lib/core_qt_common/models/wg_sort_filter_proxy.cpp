#include "wg_sort_filter_proxy.hpp"
#include "qt_connection_holder.hpp"
#include "qt_item_data.hpp"
#include "core_qt_common/models/wgt_item_model_base.hpp"
#include "core_qt_common/models/role_provider.hpp"
#include "core_data_model/common_data_roles.hpp"
#include "core_dependency_system/depends.hpp"
#include "interfaces/i_qt_helpers.hpp"
#include <QMetaMethod>

namespace wgt
{
struct WGSortFilterProxy::Impl
	: public WgtItemModelBase
	, Depends< IQtHelpers >
{
	Impl( WGSortFilterProxy & filterProxy )
		: filterProxy_( filterProxy )
		, helperInitialized_( false )
		, itemModel_( nullptr )
	{
	}

	QAbstractItemModel * getSourceModel() const override
	{
		return filterProxy_.sourceModel();
	}

	QModelIndex getSourceIndex(const QModelIndex & index) const override
	{
		return filterProxy_.mapToSource(index);
	}

	QObject* sortObject_ = nullptr;
	QObject* filterObject_ = nullptr;
	QtFilterObject* nativeFilterObject_ = nullptr;
	QMetaMethod filterAcceptsRow_;
	QMetaMethod filterAcceptsColumn_;
	QMetaMethod filterAcceptsItem_;
	QMetaMethod lessThan_;
	std::shared_ptr<QtItemData::MetaObject> metaObject_;
	QtConnectionHolder connections_;
	WGSortFilterProxy & filterProxy_;
	bool helperInitialized_;
	QueryHelper helper_;
	IWgtItemModel* itemModel_;
};

WGSortFilterProxy::WGSortFilterProxy()
	: WGTInterfaceProvider( this )
	, impl_(new Impl( *this ) )
{
	registerInterface(*impl_);

	setDynamicSortFilter(false);

	QObject::connect(this, &QAbstractProxyModel::sourceModelChanged, this, &WGSortFilterProxy::onSourceModelChanged);
	QObject::connect(this, &WGSortFilterProxy::sortObjectChanged, this, &WGSortFilterProxy::onSortObjectChanged);
	QObject::connect(this, &WGSortFilterProxy::filterObjectChanged, this, &WGSortFilterProxy::onFilterObjectChanged);
}

WGSortFilterProxy::~WGSortFilterProxy()
{
	if (impl_->metaObject_ != nullptr)
	{
		impl_->metaObject_->destroy();
	}
}

QModelIndex WGSortFilterProxy::mapToSource(const QModelIndex& proxyIndex) const
{
	return QSortFilterProxyModel::mapToSource(proxyIndex);
}

QModelIndex WGSortFilterProxy::mapFromSource(const QModelIndex& sourceIndex) const
{
	return QSortFilterProxyModel::mapFromSource(sourceIndex);
}

void WGSortFilterProxy::sort(int column, Qt::SortOrder order)
{
	emit beforeSort();
	QSortFilterProxyModel::sort(column, order);
	emit afterSort();
}

void WGSortFilterProxy::invalidateFilter()
{
	QSortFilterProxyModel::recalculateFilter();
}

QObject* WGSortFilterProxy::getSortObject() const
{
	return impl_->sortObject_;
}

void WGSortFilterProxy::setSortObject(QObject* sortObject)
{
	if (impl_->sortObject_ == sortObject)
	{
		return;
	}

	impl_->sortObject_ = sortObject;
	emit sortObjectChanged();
}

QObject* WGSortFilterProxy::getFilterObject() const
{
	return impl_->filterObject_;
}

void WGSortFilterProxy::setFilterObject(QObject* filterObject)
{
	if (impl_->filterObject_ == filterObject)
	{
		return;
	}

	impl_->filterObject_ = filterObject;

	if(auto nativeFilterObject = dynamic_cast<QtFilterObject*>(filterObject))
	{
		impl_->nativeFilterObject_ = nativeFilterObject;
	}
	else
	{
		impl_->nativeFilterObject_ = nullptr;
	}

	emit filterObjectChanged();
}

void WGSortFilterProxy::onSourceModelChanged()
{
	auto model = sourceModel();
	impl_->connections_.reset();
	if (model != nullptr)
	{
		impl_->connections_ += QObject::connect(model, &QAbstractItemModel::modelReset, [this, model] {
			if (impl_->metaObject_ != nullptr)
			{
				impl_->metaObject_->destroy();
			}
			impl_->metaObject_ = QtItemData::getMetaObject(*model);
			impl_->helper_.clear();
			impl_->helperInitialized_ = false;
		});
	}
	if (impl_->metaObject_ != nullptr)
	{
		impl_->metaObject_->destroy();
	}
	impl_->metaObject_ = model != nullptr ? QtItemData::getMetaObject(*model) : nullptr;
	impl_->helper_.clear();
	impl_->helperInitialized_ = false;
}

void WGSortFilterProxy::onSortObjectChanged()
{
	impl_->lessThan_ = QMetaMethod();
	if (impl_->sortObject_ != nullptr)
	{
		auto metaObject = impl_->sortObject_->metaObject();
		auto methodIndex = metaObject->indexOfMethod("lessThan(QVariant,QVariant)");
		if (methodIndex != -1)
		{
			impl_->lessThan_ = metaObject->method(methodIndex);
		}
	}
	invalidateFilter();
}

void WGSortFilterProxy::onFilterObjectChanged()
{
	impl_->filterAcceptsRow_ = QMetaMethod();
	impl_->filterAcceptsColumn_ = QMetaMethod();
	impl_->filterAcceptsItem_ = QMetaMethod();
	if (impl_->filterObject_ != nullptr && impl_->nativeFilterObject_ == nullptr)
	{
		auto metaObject = impl_->filterObject_->metaObject();
		auto methodIndex = metaObject->indexOfMethod("filterAcceptsRow(QVariant)");
		if (methodIndex != -1)
		{
			impl_->filterAcceptsRow_ = metaObject->method(methodIndex);
		}
		methodIndex = metaObject->indexOfMethod("filterAcceptsColumn(QVariant)");
		if (methodIndex != -1)
		{
			impl_->filterAcceptsColumn_ = metaObject->method(methodIndex);
		}
		methodIndex = metaObject->indexOfMethod("filterAcceptsItem(QVariant)");
		if (methodIndex != -1)
		{
			impl_->filterAcceptsItem_ = metaObject->method(methodIndex);
		}
	}
	impl_->helper_.clear();
	impl_->helperInitialized_ = false;
	invalidateFilter();
}

WGSortFilterProxy::SortFilterResult WGSortFilterProxy::filterAcceptsRowNonRecursive(int source_row, const QModelIndex& source_parent) const
{
	if (impl_->filterAcceptsRow_.isValid())
	{
		auto index = sourceModel()->index(source_row, 0, source_parent);
		QVariant ret;
		QtItemData item(index, impl_->metaObject_);
		if (impl_->filterAcceptsRow_.invoke(impl_->filterObject_, Q_RETURN_ARG(QVariant, ret),
		                                    Q_ARG(QVariant, QVariant::fromValue(&item))))
		{
			return WGSortFilterProxy::SortFilterResult(ret.toInt());
		}
	}
	else if(impl_->nativeFilterObject_ && impl_->nativeFilterObject_->filterAcceptsRowValid())
	{
		if(!impl_->nativeFilterObject_->hasFilter())
		{
			return ACCEPTED;
		}

		auto index = sourceModel()->index(source_row, 0, source_parent);
		Variant data = indexToItemId(index);
		auto result = impl_->nativeFilterObject_->filterAcceptsRow(data);
		return result ? ACCEPTED : REJECTED;
	}

	if (impl_->filterAcceptsItem_.isValid())
	{
		bool invoked = false;
		auto columnCount = sourceModel()->columnCount(source_parent);
		for (auto column = 0; column < columnCount; ++column)
		{
			auto index = sourceModel()->index(source_row, column, source_parent);
			QVariant ret;
			QtItemData item(index, impl_->metaObject_);
			if (impl_->filterAcceptsItem_.invoke(impl_->filterObject_, Q_RETURN_ARG(QVariant, ret),
			                                     Q_ARG(QVariant, QVariant::fromValue(&item))))
			{
				invoked = true;
				if (ret.toInt() == ACCEPTED)
				{
					return ACCEPTED;
				}
			}
		}
		if (invoked)
		{
			return REJECTED;
		}
	}
	else if(impl_->nativeFilterObject_ && impl_->nativeFilterObject_->filterAcceptsItemValid())
	{
		if (!impl_->nativeFilterObject_->hasFilter())
		{
			return ACCEPTED;
		}

		auto columnCount = sourceModel()->columnCount(source_parent);
		for (auto column = 0; column < columnCount; ++column)
		{
			auto index = sourceModel()->index(source_row, column, source_parent);
			Variant data = indexToItemId(index);
			auto result = impl_->nativeFilterObject_->filterAcceptsItem(data);
			if (result)
			{
				return ACCEPTED;
			}
		}
		return REJECTED;
	}

	return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent) ? ACCEPTED : REJECTED;
}

WGSortFilterProxy::SortFilterResult WGSortFilterProxy::filterAcceptsColumnNonRecursive(int source_column, const QModelIndex& source_parent) const
{
	if (impl_->filterAcceptsColumn_.isValid())
	{
		auto index = sourceModel()->index(0, source_column, source_parent);
		QVariant ret;
		QtItemData item(index, impl_->metaObject_);
		if (impl_->filterAcceptsColumn_.invoke(impl_->filterObject_, Q_RETURN_ARG(QVariant, ret),
		                                       Q_ARG(QVariant, QVariant::fromValue(&item))))
		{
			return WGSortFilterProxy::SortFilterResult(ret.toInt());
		}
	}
	else if (impl_->nativeFilterObject_ && impl_->nativeFilterObject_->filterAcceptsColumnValid())
	{
		if (!impl_->nativeFilterObject_->hasFilter())
		{
			return ACCEPTED;
		}

		auto index = sourceModel()->index(0, source_column, source_parent);
		Variant data = indexToItemId(index);
		auto result = impl_->nativeFilterObject_->filterAcceptsColumn(data);
		return result ? ACCEPTED : REJECTED;
	}

	if (impl_->filterAcceptsItem_.isValid())
	{
		bool invoked = false;
		auto rowCount = sourceModel()->rowCount(source_parent);
		for (auto row = 0; row < rowCount; ++row)
		{
			auto index = sourceModel()->index(row, source_column, source_parent);
			QVariant ret;
			QtItemData item(index, impl_->metaObject_);
			if (impl_->filterAcceptsItem_.invoke(impl_->filterObject_, Q_RETURN_ARG(QVariant, ret),
			                                     Q_ARG(QVariant, QVariant::fromValue(&item))))
			{
				invoked = true;
				if (ret.toInt() == ACCEPTED)
				{
					return ACCEPTED;
				}
			}
		}
		if (invoked)
		{
			return REJECTED;
		}
	}
	else if (impl_->nativeFilterObject_ && impl_->nativeFilterObject_->filterAcceptsItemValid())
	{
		if (!impl_->nativeFilterObject_->hasFilter())
		{
			return ACCEPTED;
		}

		auto rowCount = sourceModel()->rowCount(source_parent);
		for (auto row = 0; row < rowCount; ++row)
		{
			auto index = sourceModel()->index(row, source_column, source_parent);
			Variant data = indexToItemId(index);
			auto result = impl_->nativeFilterObject_->filterAcceptsItem(data);
			if (result)
			{
				return ACCEPTED;
			}
		}
		return REJECTED;
	}

	return QSortFilterProxyModel::filterAcceptsColumn(source_column, source_parent) ? ACCEPTED : REJECTED;
}

bool WGSortFilterProxy::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
	auto result = filterAcceptsRowNonRecursive(source_row, source_parent);
	if(result == SortFilterResult::ACCEPTED)
	{
		return true;
	}

	if (result == SortFilterResult::REJECTED)
	{
		auto model = sourceModel();
		auto index = model->index(source_row, 0, source_parent);
		if (index.isValid())
		{
			auto rowCount = model->rowCount(index);
			for (auto row = 0; row < rowCount; ++row)
			{
				if (filterAcceptsRow(row, index))
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool WGSortFilterProxy::filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const
{
	auto result = filterAcceptsColumnNonRecursive(source_column, source_parent);
	if (result == SortFilterResult::ACCEPTED)
	{
		return true;
	}

	if (result == SortFilterResult::REJECTED)
	{
		auto model = sourceModel();
		auto rowCount = model->rowCount(source_parent);
		for (auto row = 0; row < rowCount; ++row)
		{
			auto index = model->index(row, 0, source_parent);
			auto columnCount = model->columnCount(index);
			if (source_column < columnCount)
			{
				if (filterAcceptsColumn(source_column, index))
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool WGSortFilterProxy::lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const
{
	if (impl_->lessThan_.isValid())
	{
		QVariant ret;
		QtItemData left(source_left, impl_->metaObject_);
		QtItemData right(source_right, impl_->metaObject_);
		if (impl_->lessThan_.invoke(impl_->sortObject_, Q_RETURN_ARG(QVariant, ret),
		                            Q_ARG(QVariant, QVariant::fromValue(&left)),
		                            Q_ARG(QVariant, QVariant::fromValue(&right))))
		{
			return ret.toBool();
		}
	}

	return QSortFilterProxyModel::lessThan(source_left, source_right);
}

Variant WGSortFilterProxy::indexToItemId(const QModelIndex& index) const
{
	if (!impl_->helperInitialized_)
	{
		impl_->helperInitialized_ = true;
		if (sourceModel() != nullptr)
		{
			auto interfaceProvider = IWGTInterfaceProvider::getInterfaceProvider(*sourceModel());
			auto wgtItemModel = interfaceProvider ? interfaceProvider->queryInterface<IWgtItemModel>() : nullptr;
			impl_->itemModel_ = wgtItemModel && wgtItemModel->canUse(impl_->helper_) ? wgtItemModel : nullptr;
		}
	}

	if (impl_->itemModel_)
	{
		impl_->helper_.reset();
		return impl_->itemModel_->variantData(
			impl_->helper_, index, RoleProvider::convertRole(ItemRole::itemIdId));
	}
	else
	{
		auto id = sourceModel()->data(index, RoleProvider::convertRole(ItemRole::itemIdId));
		auto qtHelpers = impl_->get<IQtHelpers>();
		TF_ASSERT(qtHelpers != nullptr);
		return qtHelpers->toVariant(id);
	}
}

// TODO(aidan): We're currently forcing users to pick between drag and drop and filter proxy.
//			   We need to implement a reasonable moveRows that modifies the mapping to reflect drags
}