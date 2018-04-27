#ifndef GRID_EXTENSION_HPP
#define GRID_EXTENSION_HPP

#include "list_extension.hpp"

#include <QItemSelection>

namespace wgt
{
class GridExtension : public ListExtension
{
	Q_OBJECT

public:
	GridExtension();
	virtual ~GridExtension();

	Q_PROPERTY(int flowCount MEMBER flowCount_)

	Q_INVOKABLE QModelIndex getForwardIndex(const QModelIndex& index, QAbstractItemModel* pModel) const;

	Q_INVOKABLE QModelIndex getBackwardIndex(const QModelIndex& index, QAbstractItemModel* pModel) const;

private:
	int flowCount_;
};
} // end namespace wgt
#endif // GRID_EXTENSION_HPP
