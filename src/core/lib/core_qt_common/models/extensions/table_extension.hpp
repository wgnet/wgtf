#ifndef TABLE_EXTENSION_HPP
#define TABLE_EXTENSION_HPP

#include "list_extension.hpp"

#include <QItemSelection>

namespace wgt
{
class TableExtension : public ListExtension
{
	Q_OBJECT

public:
	TableExtension();
	virtual ~TableExtension();

	/**
	 *	Move to the next index.
	 *	@param index to be moved.
	 *	@param pModel to be used if index is invalid.
	 *	@return new position or
	 *		the first index in the model if nothing was selected or
	 *		the original index on error.
	 */
	Q_INVOKABLE QModelIndex getForwardIndex(const QModelIndex& index, QAbstractItemModel* pModel) const;

	/**
	 *	Move to the previous index.
	 *	@param index to be moved.
	 *	@param pModel to be used if index is invalid.
	 *	@return new position or
	 *		the first index in the model if nothing was selected or
	 *		the original index on error.
	 */
	Q_INVOKABLE QModelIndex getBackwardIndex(const QModelIndex& index, QAbstractItemModel* pModel) const;
};
} // end namespace wgt
#endif // TABLE_EXTENSION_HPP
