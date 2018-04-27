#include "tree_extension.hpp"

#include "core_common/assert.hpp"
#include "core_qt_common/models/adapters/child_list_adapter.hpp"
#include "core_qt_common/models/adapters/indexed_adapter.hpp"
#include "core_variant/variant.hpp"
#include "core_qt_common/i_qt_framework.hpp"
#include "core_common/scoped_stop_watch.hpp"
#include "core_reflection/reflection_batch_query.hpp"

namespace wgt
{
ITEMROLE(childModel)
ITEMROLE(hasChildren)
ITEMROLE(expanded)

struct TreeExtension::Implementation
{
	Implementation(TreeExtension& self);
	~Implementation();
	bool expanded(const QModelIndex& index) const;

	TreeExtension& self_;
	std::vector<IndexedAdapter<ChildListAdapter>> childModels_;
	std::vector<std::unique_ptr<ChildListAdapter>> redundantChildModels_;

	struct ExpandHelper
	{
		ExpandHelper(TreeExtension::Implementation & impl, bool forceExpand);
		~ExpandHelper();

		TreeExtension::Implementation & impl_;
		bool forceExpand_;
		//TODO : move to core_data_model and hook a signal to tree extension
		ReflectionBatchQuery batchQuery_;
	};

	ExpandHelper * expandHelper_;
};


//------------------------------------------------------------------------------
TreeExtension::Implementation::ExpandHelper::ExpandHelper(
	TreeExtension::Implementation & impl, bool forceExpand)
	: impl_(impl)
	, forceExpand_(forceExpand)
{
	impl_.expandHelper_ = forceExpand_ ? this : nullptr;
}


//------------------------------------------------------------------------------
TreeExtension::Implementation::ExpandHelper::~ExpandHelper()
{
	impl_.expandHelper_ = nullptr;
}


//------------------------------------------------------------------------------
TreeExtension::Implementation::Implementation(TreeExtension& self)
	: self_(self)
	, expandHelper_( nullptr )
{
}

TreeExtension::Implementation::~Implementation()
{
}

bool TreeExtension::Implementation::expanded(const QModelIndex& index) const
{
	auto currentState = self_.extensionData_->data(index, ItemRole::expandedId).toBool();
	//TODO: Make this work for the situation where we need to expand only some properties 
	//especially when we switch between documents / resetting the model.
	if (expandHelper_)
	{
		if (currentState == false)
		{
			self_.extensionData_->suppressNotifications(true);
			self_.extensionData_->setData(
				index, true, ItemRole::expandedId);
			self_.extensionData_->suppressNotifications(false);
			return true;
		}
	}
	return currentState;
}

TreeExtension::TreeExtension() : impl_(new Implementation(*this))
{
	roles_.push_back(ItemRole::childModelName);
	roles_.push_back(ItemRole::hasChildrenName);
	roles_.push_back(ItemRole::expandedName);
}

TreeExtension::~TreeExtension()
{
}

QVariant TreeExtension::data(const QModelIndex& index, ItemRole::Id roleId) const
{
	auto model = index.model();
	if (model == nullptr)
	{
		return QVariant(QVariant::Invalid);
	}

	if (roleId == ItemRole::childModelId)
	{
		if (!model->hasChildren(index))
		{
			return QVariant(QVariant::Invalid);
		}

		auto it = std::find(impl_->childModels_.begin(), impl_->childModels_.end(), index);
		if (it != impl_->childModels_.end())
		{
			return QVariant::fromValue<QAbstractItemModel*>(it->data_.get());
		}
		else
		{
			auto pChildModel = new ChildListAdapter(index, false);
			impl_->childModels_.emplace_back(index, pChildModel);
			return QVariant::fromValue<QAbstractItemModel*>(pChildModel);
		}
	}
	else if (roleId == ItemRole::hasChildrenId)
	{
		return model->hasChildren(index);
	}
	else if (roleId == ItemRole::expandedId)
	{
		return impl_->expanded(index);
	}

	return QVariant(QVariant::Invalid);
}

bool TreeExtension::setData(const QModelIndex& index, const QVariant& value, ItemRole::Id roleId)
{
	auto model = index.model();
	if (model == nullptr)
	{
		return false;
	}

	if (roleId == ItemRole::expandedId)
	{
		return extensionData_->setData(index, value, roleId);
	}

	return false;
}

void TreeExtension::onLayoutAboutToBeChanged(const QList<QPersistentModelIndex>& parents,
                                             QAbstractItemModel::LayoutChangeHint hint)
{
	isolateRedundantIndices(impl_->childModels_, impl_->redundantChildModels_);
	if (parents.empty())
	{
		isolateRedundantIndices(QModelIndex(), impl_->childModels_, impl_->redundantChildModels_);
	}
	else
	{
		for (auto it = parents.begin(); it != parents.end(); ++it)
		{
			isolateRedundantIndices(*it, impl_->childModels_, impl_->redundantChildModels_);
		}
	}

	// Iterate to initial size of childModels as more models can be added to this list
	// over the course of these signals
	size_t count = impl_->childModels_.size();
	for (size_t i = 0; i < count; ++i)
	{
		impl_->childModels_[i].data_->onParentLayoutAboutToBeChanged(parents, hint);
	}
}

void TreeExtension::onLayoutChanged(const QList<QPersistentModelIndex>& parents,
                                    QAbstractItemModel::LayoutChangeHint hint)
{
	// Iterate to initial size of childModels as more models can be added to this list
	// over the course of these signals
	size_t count = impl_->childModels_.size();
	for (size_t i = 0; i < count; ++i)
	{
		impl_->childModels_[i].data_->onParentLayoutChanged(parents, hint);
	}

	impl_->redundantChildModels_.clear();

	QVector<ItemRole::Id> extRoles;
	extRoles.append(ItemRole::childModelId);
	extRoles.append(ItemRole::hasChildrenId);
	for (auto it = parents.begin(); it != parents.end(); ++it)
	{
		emit extensionData_->dataChanged(*it, *it, extRoles);
	}
}

void TreeExtension::onRowsAboutToBeInserted(const QModelIndex& parent, int first, int last)
{
	QList<QPersistentModelIndex> parents;
	parents.append(parent);
	QAbstractItemModel::LayoutChangeHint hint = QAbstractItemModel::VerticalSortHint;

	// Iterate to initial size of childModels as more models can be added to this list
	// over the course of these signals
	size_t count = impl_->childModels_.size();
	for (size_t i = 0; i < count; ++i)
	{
		impl_->childModels_[i].data_->onParentLayoutAboutToBeChanged(parents, hint);
	}
}

void TreeExtension::onRowsInserted(const QModelIndex& parent, int first, int last)
{
	QList<QPersistentModelIndex> parents;
	parents.append(parent);
	QAbstractItemModel::LayoutChangeHint hint = QAbstractItemModel::VerticalSortHint;

	// Iterate to initial size of childModels as more models can be added to this list
	// over the course of these signals
	size_t count = impl_->childModels_.size();
	for (size_t i = 0; i < count; ++i)
	{
		impl_->childModels_[i].data_->onParentLayoutChanged(parents, hint);
	}

	const auto model = parent.model();
	if (model == nullptr)
	{
		return;
	}

	if (model->rowCount(parent) == last - first + 1)
	{
		QVector<ItemRole::Id> extRoles;
		extRoles.append(ItemRole::hasChildrenId);
		extRoles.append(ItemRole::expandedId);
		emit extensionData_->dataChanged(parent, parent, extRoles);
	}
}

void TreeExtension::onRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
{
	isolateRedundantIndices(parent, first, last, impl_->childModels_, impl_->redundantChildModels_);

	QList<QPersistentModelIndex> parents;
	parents.append(parent);
	QAbstractItemModel::LayoutChangeHint hint = QAbstractItemModel::VerticalSortHint;

	// Iterate to initial size of childModels as more models can be added to this list
	// over the course of these signals
	size_t count = impl_->childModels_.size();
	for (size_t i = 0; i < count; ++i)
	{
		impl_->childModels_[i].data_->onParentLayoutAboutToBeChanged(parents, hint);
	}
}

void TreeExtension::onRowsRemoved(const QModelIndex& parent, int first, int last)
{
	QList<QPersistentModelIndex> parents;
	parents.append(parent);
	QAbstractItemModel::LayoutChangeHint hint = QAbstractItemModel::VerticalSortHint;

	// Iterate to initial size of childModels as more models can be added to this list
	// over the course of these signals
	size_t count = impl_->childModels_.size();
	for (size_t i = 0; i < count; ++i)
	{
		impl_->childModels_[i].data_->onParentLayoutChanged(parents, hint);
	}

	impl_->redundantChildModels_.clear();

	const auto model = parent.model();
	if (model == nullptr)
	{
		return;
	}

	if (model->rowCount(parent) == 0)
	{
		QVector<ItemRole::Id> extRoles;
		extRoles.append(ItemRole::hasChildrenId);
		emit extensionData_->dataChanged(parent, parent, extRoles);
	}
}

void TreeExtension::onRowsAboutToBeMoved(const QModelIndex& sourceParent, int sourceFirst, int sourceLast,
                                         const QModelIndex& destinationParent, int destinationRow) /* override */
{
	isolateRedundantIndices(sourceParent, sourceFirst, sourceLast, impl_->childModels_, impl_->redundantChildModels_);
}

void TreeExtension::onRowsMoved(const QModelIndex& sourceParent, int sourceFirst, int sourceLast,
                                const QModelIndex& destinationParent, int destinationRow) /* override */
{
	impl_->redundantChildModels_.clear();
}

void TreeExtension::onModelReset()
{
	impl_->redundantChildModels_.clear();
	isolateRedundantIndices(impl_->childModels_, impl_->redundantChildModels_);
}

QItemSelection TreeExtension::itemSelection(const QModelIndex& first, const QModelIndex& last) const
{
	if (!first.isValid() && !last.isValid())
	{
		return QItemSelection();
	}
	if (!first.isValid() && last.isValid())
	{
		return QItemSelection(last, last);
	}
	if (first.isValid() && !last.isValid())
	{
		return QItemSelection(first, first);
	}

	auto begin = first;
	auto end = last;

	auto parent = QModelIndex();
	{
		// Check if end is a descendant of begin
		auto endTmp = end;
		while (endTmp.isValid())
		{
			if (begin == endTmp)
			{
				parent = begin;
				break;
			}
			endTmp = endTmp.parent();
		}
	}

	if (!parent.isValid())
	{
		// Check if begin is a descendant of end
		auto beginTmp = begin;
		while (beginTmp.isValid())
		{
			if (beginTmp == end)
			{
				std::swap(begin, end);
				parent = begin;
				break;
			}
			beginTmp = beginTmp.parent();
		}
	}

	if (!parent.isValid())
	{
		// Check if begin comes before end or vice versa
		auto beginTmp = begin;
		while (beginTmp.isValid())
		{
			auto beginParent = beginTmp.parent();
			auto endTmp = end;
			while (endTmp.isValid())
			{
				auto endParent = endTmp.parent();
				if (beginParent == endParent)
				{
					if (beginTmp.row() > endTmp.row())
					{
						std::swap(begin, end);
					}
					parent = beginParent;
					break;
				}
				endTmp = endParent;
			}
			if (parent.isValid())
			{
				break;
			}
			beginTmp = beginParent;
		}
	}

	// Create an item selection from begin to end
	QItemSelection itemSelection;

	auto it = begin;
	while (true)
	{
		itemSelection.select(it, it);

		if (it == end)
		{
			break;
		}

		// Move next
		const auto next = this->getNextIndex(it, const_cast<QAbstractItemModel*>(it.model()));
		TF_ASSERT(it != next);
		it = next;
	}

	return itemSelection;
}

QModelIndex TreeExtension::getNextIndex(const QModelIndex& index, QAbstractItemModel* pModel) const
{
	// If index is invalid, return an index to the first item
	// The index can be invalid if the user tabbed to the view, instead of clicking
	if (!index.isValid() || (index.model() == nullptr))
	{
		if (pModel == nullptr)
		{
			return index;
		}
		return pModel->index(0, 0);
	}
	TF_ASSERT(index.model() == pModel);

	// Move to next child
	// > a    - start
	//  | b   - end
	//  | c
	if (impl_->expanded(index) && pModel->hasChildren(index))
	{
		const auto child = index.child(0, index.column());
		if (child.isValid())
		{
			return child;
		}
	}

	auto it = index;
	while (it.isValid())
	{
		// Move to next sibling
		// > a    - start
		//  | b
		// > c    - end
		const auto parent = it.parent();
		const auto row = it.row() + 1;
		if (row < it.model()->rowCount(parent))
		{
			const auto sibling = it.sibling(row, index.column());
			if (sibling.isValid())
			{
				return sibling;
			}
		}

		// Or move to next sibling of parent
		// > a
		//  | b   - start
		// > c    - end
		it = parent;
	}

	// Could not move to next item
	return index;
}

QModelIndex TreeExtension::getPreviousIndex(const QModelIndex& index, QAbstractItemModel* pModel) const
{
	// If index is invalid, return an index to the first item
	// The index can be invalid if the user tabbed to the view, instead of clicking
	if (!index.isValid() || (index.model() == nullptr))
	{
		if (pModel == nullptr)
		{
			return index;
		}
		return pModel->index(0, 0);
	}
	TF_ASSERT(index.model() == pModel);

	// Move back to previous sibling
	// > a
	//  | b   - end
	//  | c   - start
	const auto row = index.row() - 1;
	if (row >= 0)
	{
		const auto sibling = index.sibling(row, index.column());
		if (sibling.isValid())
		{
			// Move forward to last child in the sibling
			// > a
			//  > b
			//    | d
			//    | e - end
			//  | c   - start
			auto it = sibling;
			while (it.isValid())
			{
				// Has children, move to last one
				if (impl_->expanded(it) && pModel->hasChildren(it))
				{
					const int lastRow = it.model()->rowCount(it) - 1;
					const auto lastChild = it.child(lastRow, index.column());
					if (lastChild.isValid())
					{
						it = lastChild;
					}
					else
					{
						// Last child not valid
						return it;
					}
				}
				else
				{
					// Previous row does not have children expanded
					return it;
				}
			}
		}
	}

	// Move to previous parent
	// > a
	//  > b   - end
	//    | d - start
	//    | e
	//  | c
	const auto parent = index.parent();
	if (parent.isValid())
	{
		return parent;
	}

	// Could not move to previous item
	return index;
}

QModelIndex TreeExtension::getForwardIndex(const QModelIndex& index, QAbstractItemModel* pModel) const
{
	// If index is invalid, return an index to the first item
	// The index can be invalid if the user tabbed to the view, instead of clicking
	if (!index.isValid() || (index.model() == nullptr))
	{
		if (pModel == nullptr)
		{
			return index;
		}
		return pModel->index(0, 0);
	}
	TF_ASSERT(index.model() == pModel);

	// Make sure the current item has children
	if (pModel->hasChildren(index))
	{
		if (impl_->expanded(index))
		{
			// Select the first child if the current item is expanded
			const auto child = index.child(0, index.column());
			if (child.isValid())
			{
				return child;
			}
		}
		else
		{
			// Expand the current item
			const_cast<TreeExtension*>(this)->extensionData_->setData(index, true, ItemRole::expandedId);
			return index;
		}
	}

	return this->getNextIndex(index, pModel);
}

QModelIndex TreeExtension::getBackwardIndex(const QModelIndex& index, QAbstractItemModel* pModel) const
{
	// If index is invalid, return an index to the first item
	// The index can be invalid if the user tabbed to the view, instead of clicking
	if (!index.isValid() || (index.model() == nullptr))
	{
		if (pModel == nullptr)
		{
			return index;
		}
		return pModel->index(0, 0);
	}
	TF_ASSERT(index.model() == pModel);

	// Move up to the parent if there are no children or not expanded
	if (pModel->hasChildren(index) && impl_->expanded(index))
	{
		// Collapse the current item
		const_cast<TreeExtension*>(this)->extensionData_->setData(index, false, ItemRole::expandedId);
		return index;
	}

	return this->getPreviousIndex(index, pModel);
}

void TreeExtension::expand(const QModelIndex& index, bool recursive)
{
	Implementation::ExpandHelper expandHelper(*impl_, recursive);
	expandInternal(index, recursive);
}

void TreeExtension::expandInternal(const QModelIndex& index, bool recursive)
{
	if (!index.isValid())
	{
		return;
	}

	const auto pModel = index.model();
	TF_ASSERT(pModel != nullptr);
	if (pModel->hasChildren(index))
	{
		extensionData_->setData(index, true, ItemRole::expandedId);
		if (recursive)
		{
			for (int r = 0; r < pModel->rowCount(index); ++r) {
				QModelIndex childIndex = pModel->index(r, 0, index);
				expand(childIndex, recursive);
			}
		}
	}
	expand(pModel->parent(index), false);
}

void TreeExtension::collapse(const QModelIndex& index)
{
	if (!index.isValid())
	{
		return;
	}

	extensionData_->setData(index, false, ItemRole::expandedId);
}

void TreeExtension::toggle(const QModelIndex& index, bool recursiveExpand)
{
	SCOPE_TAG
	if (impl_->expanded(index))
	{
		collapse(index);
	}
	else
	{
		expand(index, recursiveExpand);
	}
}
} // end namespace wgt
