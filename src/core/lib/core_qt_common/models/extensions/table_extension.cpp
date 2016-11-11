#include "table_extension.hpp"

namespace wgt
{
TableExtension::TableExtension()
{
}

TableExtension::~TableExtension()
{
}

QModelIndex TableExtension::getForwardIndex(const QModelIndex& index, QAbstractItemModel* pModel) const
{
	if (!index.isValid() || (index.model() == nullptr))
	{
		if (pModel == nullptr)
		{
			return index;
		}
		return pModel->index(0, 0);
	}
	assert(index.model() == pModel);

	if (index.column() >= (pModel->columnCount(index.parent()) - 1))
	{
		return index;
	}
	return pModel->index(index.row(), index.column() + 1, index.parent());
}

QModelIndex TableExtension::getBackwardIndex(const QModelIndex& index, QAbstractItemModel* pModel) const
{
	if (!index.isValid() || (index.model() == nullptr))
	{
		if (pModel == nullptr)
		{
			return index;
		}
		return pModel->index(0, 0);
	}
	assert(index.model() == pModel);

	if (index.column() <= 0)
	{
		return index;
	}
	return pModel->index(index.row(), index.column() - 1, index.parent());
}
} // end namespace wgt
