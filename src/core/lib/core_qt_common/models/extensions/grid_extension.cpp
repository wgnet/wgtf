#include "grid_extension.hpp"

#include "core_common/assert.hpp"

namespace wgt
{
GridExtension::GridExtension()
: flowCount_(-1)
{
}

GridExtension::~GridExtension()
{
}

QModelIndex GridExtension::getForwardIndex(const QModelIndex& index, QAbstractItemModel* pModel) const
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

	auto maxIndex = pModel->rowCount() - 1;
	if (flowCount_ <= 0 || index.row() / flowCount_ == maxIndex / flowCount_)
	{
		return index;
	}
	return pModel->index(std::min(index.row() + flowCount_, maxIndex), index.column(), index.parent());
}

QModelIndex GridExtension::getBackwardIndex(const QModelIndex& index, QAbstractItemModel* pModel) const
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

	if (flowCount_ <= 0 || index.row() < flowCount_)
	{
		return index;
	}
	return pModel->index(index.row() - flowCount_, index.column(), index.parent());
}
} // end namespace wgt
