#include "wg_sort_filter_proxy.hpp"
#include "qt_connection_holder.hpp"
#include "qt_item_data.hpp"

#include <QMetaMethod>

namespace wgt
{
struct WGSortFilterProxy::Impl
{
	Impl()
	    : sortObject_(nullptr)
	    , filterObject_(nullptr)
	{
	}

	QObject* sortObject_;
	QObject* filterObject_;
	QMetaMethod filterAcceptsRow_;
	QMetaMethod filterAcceptsColumn_;
	QMetaMethod filterAcceptsItem_;
	QMetaMethod lessThan_;
	std::shared_ptr<QtItemData::MetaObject> metaObject_;
	QtConnectionHolder connections_;
};

WGSortFilterProxy::WGSortFilterProxy()
    : impl_(new Impl)
{
	setDynamicSortFilter(false);

	QObject::connect(this, &QAbstractProxyModel::sourceModelChanged, this, &WGSortFilterProxy::onSourceModelChanged);
	QObject::connect(this, &WGSortFilterProxy::sortObjectChanged, this, &WGSortFilterProxy::onSortObjectChanged);
	QObject::connect(this, &WGSortFilterProxy::filterObjectChanged, this, &WGSortFilterProxy::onFilterObjectChanged);
}

WGSortFilterProxy::~WGSortFilterProxy()
{
}

void WGSortFilterProxy::sort(int column, Qt::SortOrder order)
{
	QSortFilterProxyModel::sort(column, order);
}

void WGSortFilterProxy::invalidateFilter()
{
	QSortFilterProxyModel::invalidateFilter();
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
	emit filterObjectChanged();
}

void WGSortFilterProxy::onSourceModelChanged()
{
	auto model = sourceModel();
	impl_->connections_.reset();
	if (model != nullptr)
	{
		impl_->connections_ += QObject::connect(model, &QAbstractItemModel::modelReset,
		                                        [this, model] { impl_->metaObject_ = QtItemData::getMetaObject(*model); });
	}
	impl_->metaObject_ = model != nullptr ? QtItemData::getMetaObject(*model) : nullptr;
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
	if (impl_->filterObject_ != nullptr)
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
	invalidateFilter();
}

bool WGSortFilterProxy::filterAcceptsRowNonRecursive(int source_row, const QModelIndex& source_parent) const
{
	if (impl_->filterAcceptsRow_.isValid())
	{
		auto index = sourceModel()->index(source_row, 0, source_parent);
		QVariant ret;
		auto item = std::unique_ptr<QObject>(new QtItemData(index, impl_->metaObject_));
		if (impl_->filterAcceptsRow_.invoke(impl_->filterObject_,
		                                    Q_RETURN_ARG(QVariant, ret),
		                                    Q_ARG(QVariant, QVariant::fromValue(item.get()))))
		{
			return ret.toBool();
		}
	}

	if (impl_->filterAcceptsItem_.isValid())
	{
		bool invoked = false;
		auto columnCount = sourceModel()->columnCount(source_parent);
		for (auto column = 0; column < columnCount; ++column)
		{
			auto index = sourceModel()->index(source_row, column, source_parent);
			QVariant ret;
			auto item = std::unique_ptr<QObject>(new QtItemData(index, impl_->metaObject_));
			if (impl_->filterAcceptsItem_.invoke(impl_->filterObject_,
			                                     Q_RETURN_ARG(QVariant, ret),
			                                     Q_ARG(QVariant, QVariant::fromValue(item.get()))))
			{
				invoked = true;
				if (ret.toBool())
				{
					return true;
				}
			}
		}
		if (invoked)
		{
			return false;
		}
	}

	return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

bool WGSortFilterProxy::filterAcceptsColumnNonRecursive(int source_column, const QModelIndex& source_parent) const
{
	if (impl_->filterAcceptsColumn_.isValid())
	{
		auto index = sourceModel()->index(0, source_column, source_parent);
		QVariant ret;
		auto item = std::unique_ptr<QObject>(new QtItemData(index, impl_->metaObject_));
		if (impl_->filterAcceptsColumn_.invoke(impl_->filterObject_,
		                                       Q_RETURN_ARG(QVariant, ret),
		                                       Q_ARG(QVariant, QVariant::fromValue(item.get()))))
		{
			return ret.toBool();
		}
	}

	if (impl_->filterAcceptsItem_.isValid())
	{
		bool invoked = false;
		auto rowCount = sourceModel()->rowCount(source_parent);
		for (auto row = 0; row < rowCount; ++row)
		{
			auto index = sourceModel()->index(row, source_column, source_parent);
			QVariant ret;
			auto item = std::unique_ptr<QObject>(new QtItemData(index, impl_->metaObject_));
			if (impl_->filterAcceptsItem_.invoke(impl_->filterObject_,
			                                     Q_RETURN_ARG(QVariant, ret),
			                                     Q_ARG(QVariant, QVariant::fromValue(item.get()))))
			{
				invoked = true;
				if (ret.toBool())
				{
					return true;
				}
			}
		}
		if (invoked)
		{
			return false;
		}
	}

	return QSortFilterProxyModel::filterAcceptsColumn(source_column, source_parent);
}

bool WGSortFilterProxy::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
	if (filterAcceptsRowNonRecursive(source_row, source_parent))
	{
		return true;
	}

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

	return false;
}

bool WGSortFilterProxy::filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const
{
	if (filterAcceptsColumnNonRecursive(source_column, source_parent))
	{
		return true;
	}

	auto model = sourceModel();
	auto rowCount = model->rowCount(source_parent);
	for (auto row = 0; row < rowCount; ++row)
	{
		auto index = model->index(row, 0, source_parent);
		if (filterAcceptsColumn(source_column, index))
		    {
				return true;
			}
		}

	    return false;
}

bool WGSortFilterProxy::lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const
{
	if (impl_->lessThan_.isValid())
	{
		QVariant ret;
		auto left = std::unique_ptr<QObject>(new QtItemData(source_left, impl_->metaObject_));
		auto right = std::unique_ptr<QObject>(new QtItemData(source_right, impl_->metaObject_));
		if (impl_->lessThan_.invoke(impl_->sortObject_,
		                            Q_RETURN_ARG(QVariant, ret),
		                            Q_ARG(QVariant, QVariant::fromValue(left.get())),
		                            Q_ARG(QVariant, QVariant::fromValue(right.get()))))
		{
			return ret.toBool();
		}
	}

	return QSortFilterProxyModel::lessThan(source_left, source_right);
}

//TODO(aidan): We're currently forcing users to pick between drag and drop and filter proxy.
//			   We need to implement a reasonable moveRows that modifies the mapping to reflect drags
}