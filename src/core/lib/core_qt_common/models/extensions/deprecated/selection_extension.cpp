#include "selection_extension.hpp"
#include <cassert>
#include <set>

#include "qt_qlist_memory_fix.hpp"

SPECIALIZE_QLIST(QVariant)

namespace wgt
{
struct SelectionExtension::Implementation
{
	Implementation(SelectionExtension& self);

	QModelIndex findNextIndex(const QAbstractItemModel* model, const QModelIndex& index,
	                          bool skipChildren = false) const;
	std::vector<QModelIndex> findRange(const QAbstractItemModel* model, const QModelIndex& from,
	                                   const QModelIndex& to) const;

	void select(const QModelIndex& index);
	void selectRange(const QModelIndex& index);
	void deselect(const QModelIndex& index);
	bool selected(const QModelIndex& index);

	void fireDataChangedEvent(const QModelIndex& index);
	QModelIndex firstColumnIndex(const QModelIndex& index);
	QVector<int>& selectionRoles();
	int expandedRole() const;

	QModelIndex lastSelectedIndex() const;
	bool clearPreviousSelection();

	void onRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last);
	void onRowsRemoved(const QModelIndex& parent, int first, int last);

	void onLayoutAboutToBeChanged(const QList<QPersistentModelIndex>& parents,
	                              QAbstractItemModel::LayoutChangeHint hint);
	void onLayoutChanged(const QList<QPersistentModelIndex>& parents, QAbstractItemModel::LayoutChangeHint hint);

	void resetSelectedItem();

	SelectionExtension& self_;
	QPersistentModelIndex lastClickedIndex_;
	quintptr selectedItem_;
	bool allowMultiSelect_;
	bool selectRange_;
	bool clearOnNextSelect_;
	std::set<QPersistentModelIndex> selection_;
	std::set<QPersistentModelIndex> pendingRemovingSelection_;
	QVector<int> selectionRoles_;

	QModelIndex currentIndex_;
};

SelectionExtension::Implementation::Implementation(SelectionExtension& self)
    : self_(self), selectedItem_(0), allowMultiSelect_(false), selectRange_(false), clearOnNextSelect_(false)
{
}

QModelIndex SelectionExtension::Implementation::findNextIndex(const QAbstractItemModel* model, const QModelIndex& index,
                                                              bool skipChildren) const
{
	if (index.isValid())
	{
		QModelIndex next;

		if (!skipChildren)
		{
			bool expanded = model->data(index, expandedRole()).toBool();

			if (expanded)
			{
				next = index.child(0, 0);

				if (next.isValid())
				{
					return next;
				}
			}
		}

		next = index.parent();
		int nextRow = index.row() + 1;

		if (nextRow < model->rowCount(next))
		{
			return index.sibling(nextRow, 0);
		}
		else if (next.isValid())
		{
			return findNextIndex(model, next, true);
		}
	}

	return QModelIndex();
}

std::vector<QModelIndex> SelectionExtension::Implementation::findRange(const QAbstractItemModel* model,
                                                                       const QModelIndex& from,
                                                                       const QModelIndex& to) const
{
	QModelIndex position = model->index(0, 0, QModelIndex());
	std::vector<QModelIndex> indices;
	bool inRange = false;

	for (; position.isValid(); position = findNextIndex(model, position))
	{
		if (position == from || position == to)
		{
			indices.push_back(position);

			if (inRange || from == to)
			{
				inRange = false;
				break;
			}

			inRange = true;
		}
		else if (inRange)
		{
			indices.push_back(position);
		}
	}

	if (inRange)
	{
		indices.clear();
	}

	return indices;
}

/**
 *	Update the selection data on every item in the row.
 *	Only change the selection if the row has changed.
 *	e.g.
 *							v- Clicked on Item(0,2)
 *	+---------+---------+---------+---------+
 *	|Item(0,0)|Item(0,1)|Item(0,2)|Item(0,3)| <- dataChanged for Items(0, 0-3)
 *	+---------+---------+---------+---------+
 *	|Item(1,0)|Item(1,1)|Item(1,2)|Item(1,3)|
 *	+---------+---------+---------+---------+
 *	e.g.
 *					v- Clicked on Item(0,1)
 *	+---------+---------+---------+---------+
 *	|Item(0,0)|Item(0,1)|Item(0,2)|Item(0,3)| <- same row selected as before,
 *	+---------+---------+---------+---------+    no dataChanged events
 *	|Item(1,0)|Item(1,1)|Item(1,2)|Item(1,3)|
 *	+---------+---------+---------+---------+
 */
void SelectionExtension::Implementation::select(const QModelIndex& index)
{
	if (selectRange_)
	{
		selectRange(index);
		return;
	}

	// Always use column 0
	const auto adjustedIndex = firstColumnIndex(index);
	auto inserted = selection_.insert(adjustedIndex).second;

	if (inserted)
	{
		if (!selectionRoles().empty())
		{
			if (!allowMultiSelect_)
			{
				// TODO: NGT-1004
				// find a better way to handle removing selected
				// items which were removed from selection set
				selection_.erase(lastClickedIndex_);
				if (lastClickedIndex_.isValid())
				{
					fireDataChangedEvent(lastClickedIndex_);
				}
			}

			if (adjustedIndex.isValid())
			{
				fireDataChangedEvent(adjustedIndex);
			}
		}
	}

	lastClickedIndex_ = adjustedIndex;
	selectedItem_ = adjustedIndex.internalId();
	bool somethingChanged = inserted;

	if (clearOnNextSelect_)
	{
		somethingChanged |= clearPreviousSelection();
	}

	if (somethingChanged)
	{
		emit self_.selectionChanged();
	}
}

void SelectionExtension::Implementation::selectRange(const QModelIndex& index)
{
	assert(index.isValid());

	// Always use column 0
	QModelIndex toIndex = firstColumnIndex(index);
	QModelIndex fromIndex = lastClickedIndex_;
	bool fromIndexSelected = lastSelectedIndex().isValid();
	selectRange_ = false;

	if (!lastClickedIndex_.isValid())
	{
		fromIndex = toIndex;
		lastClickedIndex_ = fromIndex;
	}

	if (!selectionRoles().empty())
	{
		const QAbstractItemModel* model = index.model();
		std::vector<QModelIndex> range = findRange(model, fromIndex, toIndex);

		if (fromIndexSelected)
		{
			selection_.erase(fromIndex);
		}

		decltype(selection_) oldSelection;
		oldSelection.swap(selection_);

		for (auto& rangeIndex : range)
		{
			selection_.insert(rangeIndex);

			if (oldSelection.erase(rangeIndex) == 0 && (rangeIndex != fromIndex || !fromIndexSelected))
			{
				fireDataChangedEvent(rangeIndex);
			}
		}

		for (auto& oldIndex : oldSelection)
		{
			if (oldIndex.isValid())
			{
				fireDataChangedEvent(oldIndex);
			}
		}
	}

	emit self_.selectionChanged();
}

void SelectionExtension::Implementation::deselect(const QModelIndex& index)
{
	assert(index.isValid());

	// Always use column 0
	const auto adjustedIndex = firstColumnIndex(index);
	auto removedIndices = selection_.erase(adjustedIndex);
	selectRange_ = false;

	if (removedIndices > 0)
	{
		if (!selectionRoles().empty())
		{
			fireDataChangedEvent(adjustedIndex);
		}

		emit self_.selectionChanged();
	}
}

/**
 *	Selection extension can only select whole rows.
 *	Do not compare columns, because that would select only one part of a row.
 */
bool SelectionExtension::Implementation::selected(const QModelIndex& index)
{
	if (!index.isValid())
	{
		return false;
	}

	// Always use column 0
	const auto adjustedIndex = firstColumnIndex(index);
	return selection_.find(adjustedIndex) != selection_.end();
}

void SelectionExtension::Implementation::fireDataChangedEvent(const QModelIndex& index)
{
	auto model = index.model();
	assert(model != nullptr);

	const auto columnCount = model->columnCount(index);
	assert(columnCount > 0);
	const auto lastColumn = columnCount - 1;

	const auto topLeft = index;
	const auto bottomRight = model->sibling(index.row(), lastColumn, index);
	assert(bottomRight.isValid());

	emit const_cast<QAbstractItemModel*>(model)->dataChanged(topLeft, bottomRight, selectionRoles());
}

QModelIndex SelectionExtension::Implementation::firstColumnIndex(const QModelIndex& index)
{
	if (!index.isValid())
	{
		return QModelIndex();
	}

	auto model = index.model();
	assert(model != nullptr);

	return index.row() == 0 ? index : model->sibling(index.row(), 0, index);
}

QVector<int>& SelectionExtension::Implementation::selectionRoles()
{
	if (selectionRoles_.empty())
	{
		int role;

		if (self_.encodeRole(SelectedRole::roleId_, role))
		{
			selectionRoles_.append(role);
		}
	}

	return selectionRoles_;
}

int SelectionExtension::Implementation::expandedRole() const
{
	static int expandedRole = -1;

	if (expandedRole < 0)
	{
		self_.encodeRole(ExpandedRole::roleId_, expandedRole);
	}

	return expandedRole;
}

QModelIndex SelectionExtension::Implementation::lastSelectedIndex() const
{
	if (selection_.find(lastClickedIndex_) != selection_.cend())
	{
		return lastClickedIndex_;
	}

	return QModelIndex();
}

bool SelectionExtension::Implementation::clearPreviousSelection()
{
	QModelIndex lastIndex = lastSelectedIndex();
	decltype(selection_) oldSelection;
	oldSelection.swap(selection_);
	bool clearedAny = false;

	if (!selectionRoles().empty())
	{
		for (auto& index : oldSelection)
		{
			if (index != lastIndex && index.isValid())
			{
				fireDataChangedEvent(index);
				clearedAny = true;
			}
		}
	}

	if (lastIndex.isValid())
	{
		selection_.insert(lastIndex);
	}

	clearOnNextSelect_ = false;
	return clearedAny;
}

void SelectionExtension::Implementation::onRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
{
	auto model = currentIndex_.model();
	if (model == nullptr)
	{
		return;
	}

	pendingRemovingSelection_.clear();
	int count = last + 1;
	for (int i = first; i < count; i++)
	{
		QModelIndex index = firstColumnIndex(model->index(i, 0, parent));
		assert(index.isValid());
		if (selected(index))
		{
			auto inserted = pendingRemovingSelection_.insert(index).second;
			assert(inserted);
		}
	}
}

void SelectionExtension::Implementation::onRowsRemoved(const QModelIndex& parent, int first, int last)
{
	bool bRemoved = false;
	for (auto pendingIndex : pendingRemovingSelection_)
	{
		if (!selectionRoles().empty())
		{
			if (lastClickedIndex_ == pendingIndex)
			{
				this->resetSelectedItem();
			}
			selection_.erase(pendingIndex);
			bRemoved = true;
		}
	}
	pendingRemovingSelection_.clear();
	if (bRemoved)
	{
		emit self_.selectionChanged();
	}
}

void SelectionExtension::Implementation::onLayoutAboutToBeChanged(const QList<QPersistentModelIndex>& parents,
                                                                  QAbstractItemModel::LayoutChangeHint hint)
{
	pendingRemovingSelection_.clear();
	for (auto selItem : selection_)
	{
		auto parent = selItem.parent();
		bool bInserted = false;
		while (parent.isValid() && !bInserted)
		{
			auto childItem = parent.internalPointer();
			parent = parent.parent();
			for (auto it = parents.cbegin(); it != parents.cend(); ++it)
			{
				auto item = static_cast<QModelIndex>(*it).internalPointer();
				if (childItem == item)
				{
					bInserted = pendingRemovingSelection_.insert(selItem).second;
					assert(bInserted);
				}
			}
		}
	}
}

void SelectionExtension::Implementation::onLayoutChanged(const QList<QPersistentModelIndex>& parents,
                                                         QAbstractItemModel::LayoutChangeHint hint)
{
	bool bRemoved = false;
	for (auto pendingIndex : pendingRemovingSelection_)
	{
		if (!selectionRoles().empty())
		{
			selection_.erase(pendingIndex);
			bRemoved = true;
		}
	}
	pendingRemovingSelection_.clear();
	if (bRemoved)
	{
		emit self_.selectionChanged();
	}
}

void SelectionExtension::Implementation::resetSelectedItem()
{
	lastClickedIndex_ = QModelIndex();
	selectedItem_ = 0;
}

SelectionExtension::SelectionExtension() : impl_(new Implementation(*this))
{
}

SelectionExtension::~SelectionExtension()
{
}

QHash<int, QByteArray> SelectionExtension::roleNames() const
{
	QHash<int, QByteArray> roleNames;
	this->registerRole(SelectedRole::roleName_, roleNames);
	this->registerRole(ExpandedRole::roleName_, roleNames);
	return roleNames;
}

QVariant SelectionExtension::data(const QModelIndex& index, int role) const
{
	ItemRole::Id roleId;
	if (!this->decodeRole(role, roleId))
	{
		return QVariant(QVariant::Invalid);
	}

	if (roleId != SelectedRole::roleId_)
	{
		return QVariant(QVariant::Invalid);
	}

	return impl_->selected(index);
}

bool SelectionExtension::setData(const QModelIndex& index, const QVariant& value, int role)
{
	ItemRole::Id roleId;
	if (!this->decodeRole(role, roleId))
	{
		return false;
	}

	if (roleId != SelectedRole::roleId_)
	{
		return false;
	}

	if (value.toBool())
	{
		impl_->select(index);
	}
	else
	{
		impl_->deselect(index);
	}

	return true;
}

void SelectionExtension::onRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
{
	impl_->onRowsAboutToBeRemoved(parent, first, last);
}

void SelectionExtension::onRowsRemoved(const QModelIndex& parent, int first, int last)
{
	impl_->onRowsRemoved(parent, first, last);
}

void SelectionExtension::onDataAboutToBeChanged(const QModelIndex& index, int role, const QVariant& value)
{
	// Does nothing
}

void SelectionExtension::onDataChanged(const QModelIndex& index, int role, const QVariant& value)
{
	// Does nothing
}

void SelectionExtension::onLayoutAboutToBeChanged(const QList<QPersistentModelIndex>& parents,
                                                  QAbstractItemModel::LayoutChangeHint hint)
{
	impl_->onLayoutAboutToBeChanged(parents, hint);
}

void SelectionExtension::onLayoutChanged(const QList<QPersistentModelIndex>& parents,
                                         QAbstractItemModel::LayoutChangeHint hint)
{
	impl_->onLayoutChanged(parents, hint);
}

void SelectionExtension::clearOnNextSelect()
{
	impl_->clearOnNextSelect_ = true;
}

void SelectionExtension::prepareRangeSelect()
{
	impl_->selectRange_ = true;
}

QVariant SelectionExtension::getSelectedIndex() const
{
	return QVariant::fromValue(impl_->lastSelectedIndex());
}

QVariant SelectionExtension::getSelectedItem() const
{
	return QVariant::fromValue(impl_->selectedItem_);
}

QList<QVariant> SelectionExtension::getSelection() const
{
	QList<QVariant> selection;

	for (auto& index : impl_->selection_)
	{
		selection.append(QModelIndex(index));
	}

	return selection;
}

bool SelectionExtension::indexInSelection(const QVariant& index)
{
	QModelIndex idx = index.toModelIndex();

	for (auto& index : impl_->selection_)
	{
		QModelIndex current(index);
		if (current == idx)
		{
			return true;
		}
	}

	return false;
}

void SelectionExtension::setSelectedIndex(const QVariant& index)
{
	QModelIndex idx = index.toModelIndex();
	impl_->select(idx);
}

bool SelectionExtension::getMultiSelect() const
{
	return impl_->allowMultiSelect_;
}

void SelectionExtension::setMultiSelect(bool value)
{
	impl_->allowMultiSelect_ = value;
	emit multiSelectChanged();

	if (!value && impl_->selection_.size() > 1)
	{
		if (impl_->clearPreviousSelection())
		{
			emit selectionChanged();
		}
	}
}

/// Move to previous index
bool SelectionExtension::moveUp()
{
	int prevRow = impl_->currentIndex_.row() - 1;

	if (0 <= prevRow)
	{
		// Update Selected role before update the current index
		selectCurrentIndex(false);
		impl_->currentIndex_ = impl_->currentIndex_.sibling(prevRow, 0);
		selectCurrentIndex(true);
		emit currentIndexChanged();
		return true;
	}

	return false;
}

/// Move to next index
bool SelectionExtension::moveDown()
{
	auto model = impl_->currentIndex_.model();
	assert(model != nullptr);

	QModelIndex parent = impl_->currentIndex_.parent();

	int nextRow = impl_->currentIndex_.row() + 1;
	if (nextRow < model->rowCount(parent))
	{
		// Update Selected role before update the current index
		selectCurrentIndex(false);
		impl_->currentIndex_ = impl_->currentIndex_.sibling(nextRow, 0);
		selectCurrentIndex(true);
		emit currentIndexChanged();
		return true;
	}

	return false;
}

QVariant SelectionExtension::getCurrentIndex() const
{
	return QVariant::fromValue(impl_->currentIndex_);
}

void SelectionExtension::setCurrentIndex(const QVariant& index)
{
	QModelIndex idx = index.toModelIndex();
	impl_->currentIndex_ = idx;

	emit currentIndexChanged();
}

/// Helper function, turn on/off the current index's Selected role
void SelectionExtension::selectCurrentIndex(bool select)
{
	int selectedRole = -1;
	this->encodeRole(SelectedRole::roleId_, selectedRole);
	setData(impl_->currentIndex_, QVariant(select), selectedRole);
}
} // end namespace wgt
