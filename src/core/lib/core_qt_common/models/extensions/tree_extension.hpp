#ifndef TREE_EXTENSION_HPP
#define TREE_EXTENSION_HPP

#include "i_model_extension.hpp"
#include <QItemSelection>
#include <memory>

namespace wgt
{
class TreeExtension : public IModelExtension
{
	Q_OBJECT

public:
	TreeExtension();
	virtual ~TreeExtension();

	QHash< int, QByteArray > roleNames() const override;
	QVariant data( const QModelIndex &index, int role ) const override;
	bool setData( const QModelIndex &index,
		const QVariant &value,
		int role ) override;

	void onLayoutAboutToBeChanged(
		const QList< QPersistentModelIndex > & parents, 
		QAbstractItemModel::LayoutChangeHint hint ) override;
	void onLayoutChanged(
		const QList< QPersistentModelIndex > & parents, 
		QAbstractItemModel::LayoutChangeHint hint ) override;
	void onRowsAboutToBeRemoved(
		const QModelIndex& parent, int first, int last ) override;
	void onRowsRemoved( 
		const QModelIndex & parent, int first, int last ) override;

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

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif // TREE_EXTENSION_HPP
