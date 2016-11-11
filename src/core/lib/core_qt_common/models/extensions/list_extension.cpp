#include "list_extension.hpp"

namespace wgt
{
ListExtension::ListExtension()
{
}

ListExtension::~ListExtension()
{
}

QItemSelection ListExtension::itemSelection(const QModelIndex& first, const QModelIndex& last) const
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

	if (begin.row() > end.row())
	{
		std::swap(begin, end);
	}

	return QItemSelection(begin, end);
}

QModelIndex ListExtension::getNextIndex(const QModelIndex& index, QAbstractItemModel* pModel) const
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
	assert(index.model() == pModel);

	if (index.row() >= (pModel->rowCount(index.parent()) - 1))
	{
		return index;
	}
	return pModel->index(index.row() + 1, index.column(), index.parent());
}

QModelIndex ListExtension::getPreviousIndex(const QModelIndex& index, QAbstractItemModel* pModel) const
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
	assert(index.model() == pModel);

	if (index.row() <= 0)
	{
		return index;
	}
	return pModel->index(index.row() - 1, index.column(), index.parent());
}
} // end namespace wgt
