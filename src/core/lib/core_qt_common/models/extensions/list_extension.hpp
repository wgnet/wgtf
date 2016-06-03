#ifndef LIST_EXTENSION_HPP
#define LIST_EXTENSION_HPP

#include "i_model_extension.hpp"

#include <QItemSelection>

namespace wgt
{
class ListExtension : public IModelExtension
{
	Q_OBJECT

public:
	ListExtension();
	virtual ~ListExtension();

	QHash< int, QByteArray > roleNames() const override;

	/**
	 *	Convert first and last index into a selection range.
	 *	@param first start of selected area.
	 *	@param last end of selected area. End can be before begin.
	 *	@return area covered inbetween first and last.
	 */
	Q_INVOKABLE QItemSelection itemSelection( const QModelIndex & first, const QModelIndex & last ) const;

	/**
	 *	Move index down one row in the list.
	 *	Index will not be moved past the end of the list.
	 *	@param index to be moved.
	 *	@return new position or the original index on error.
	 */
	Q_INVOKABLE QModelIndex getNextIndex( const QModelIndex & index ) const;

	/**
	 *	Move index up one row in the list.
	 *	Index will not be moved past the start of the list.
	 *	@param index to be moved.
	 *	@return new position or the original index on error.
	 */
	Q_INVOKABLE QModelIndex getPreviousIndex( const QModelIndex & index ) const;
};
} // end namespace wgt
#endif // LIST_EXTENSION_HPP
