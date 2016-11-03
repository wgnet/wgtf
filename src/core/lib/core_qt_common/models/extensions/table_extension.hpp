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
	 *	Expand the current item if it is expandable or move to the next index.
	 *	@param index to be moved.
	 *	@return new position or the original index if the item was expanded or on error.
	 */
	Q_INVOKABLE QModelIndex getForwardIndex( const QModelIndex & index ) const;

	/**
	 *	Collapse the current item if it is collapsable or move to the previous index.
	 *	@param index to be moved.
	 *	@return new position or the original index if the item was expanded or on error.
	 */
	Q_INVOKABLE QModelIndex getBackwardIndex( const QModelIndex & index ) const;
};
} // end namespace wgt
#endif // TABLE_EXTENSION_HPP
