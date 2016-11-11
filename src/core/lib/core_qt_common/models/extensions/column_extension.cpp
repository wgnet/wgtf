#include "column_extension.hpp"
#include "core_qt_common/models/adapters/column_list_adapter.hpp"
#include "core_qt_common/models/adapters/indexed_adapter.hpp"

namespace wgt
{
ITEMROLE(columnModel)

struct ColumnExtension::Implementation
{
	std::vector<IndexedAdapter<ColumnListAdapter>> columnModels_;
	std::vector<std::unique_ptr<ColumnListAdapter>> redundantColumnModels_;
};

ColumnExtension::ColumnExtension() : impl_(new Implementation())
{
	roles_.push_back(ItemRole::columnModelName);
}

ColumnExtension::~ColumnExtension()
{
}

QVariant ColumnExtension::data(const QModelIndex& index, ItemRole::Id roleId) const
{
	if (roleId != ItemRole::columnModelId)
	{
		return QVariant(QVariant::Invalid);
	}

	auto it = std::find_if(impl_->columnModels_.begin(), impl_->columnModels_.end(),
	                       [index](const IndexedAdapter<ColumnListAdapter>& item) {
		                       QModelIndex itemIndex = item.index_;
		                       return itemIndex.row() == index.row() && itemIndex.parent() == index.parent();
		                   });
	if (it != impl_->columnModels_.end())
	{
		return QVariant::fromValue<QAbstractItemModel*>(it->data_.get());
	}
	else
	{
		auto pColumnModel = new ColumnListAdapter(index);
		impl_->columnModels_.emplace_back(index, pColumnModel);
		return QVariant::fromValue<QAbstractItemModel*>(pColumnModel);
	}
}

void ColumnExtension::onLayoutAboutToBeChanged(const QList<QPersistentModelIndex>& parents,
                                               QAbstractItemModel::LayoutChangeHint hint)
{
	isolateRedundantIndices(impl_->columnModels_, impl_->redundantColumnModels_);
	if (parents.empty())
	{
		isolateRedundantIndices(QModelIndex(), impl_->columnModels_, impl_->redundantColumnModels_);
	}
	else
	{
		for (auto it = parents.begin(); it != parents.end(); ++it)
		{
			isolateRedundantIndices(*it, impl_->columnModels_, impl_->redundantColumnModels_);
		}
	}
}

void ColumnExtension::onLayoutChanged(const QList<QPersistentModelIndex>& parents,
                                      QAbstractItemModel::LayoutChangeHint hint)
{
	impl_->redundantColumnModels_.clear();
}

void ColumnExtension::onRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
{
	isolateRedundantIndices(parent, first, last, impl_->columnModels_, impl_->redundantColumnModels_);
}

void ColumnExtension::onRowsRemoved(const QModelIndex& parent, int first, int last)
{
	impl_->redundantColumnModels_.clear();
}

} // end namespace wgt
