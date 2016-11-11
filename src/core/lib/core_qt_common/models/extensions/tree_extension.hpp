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

	QVariant data(const QModelIndex& index, ItemRole::Id roleId) const override;
	bool setData(const QModelIndex& index, const QVariant& value, ItemRole::Id roleId) override;

	void onLayoutAboutToBeChanged(const QList<QPersistentModelIndex>& parents,
	                              QAbstractItemModel::LayoutChangeHint hint) override;
	void onLayoutChanged(const QList<QPersistentModelIndex>& parents,
	                     QAbstractItemModel::LayoutChangeHint hint) override;
	void onRowsAboutToBeInserted(const QModelIndex& parent, int first, int last) override;
	void onRowsInserted(const QModelIndex& parent, int first, int last) override;
	void onRowsAboutToBeRemoved(const QModelIndex& parent, int first, int last) override;
	void onRowsRemoved(const QModelIndex& parent, int first, int last) override;
	virtual void onRowsAboutToBeMoved(const QModelIndex& sourceParent, int sourceFirst, int sourceLast,
	                                  const QModelIndex& destinationParent, int destinationRow) override;
	virtual void onRowsMoved(const QModelIndex& sourceParent, int sourceFirst, int sourceLast,
	                         const QModelIndex& destinationParent, int destinationRow) override;

	Q_INVOKABLE QItemSelection itemSelection(const QModelIndex& first, const QModelIndex& last) const;

	/**
	 *	Move index down one row in the list.
	 *	Index will not be moved past the end of the list.
	 *	@param index to be moved.
	 *	@param pModel to be used if index is invalid.
	 *	@return new position or
	 *		the first index in the model if nothing was selected or
	 *		the original index on error.
	 */
	Q_INVOKABLE QModelIndex getNextIndex(const QModelIndex& index, QAbstractItemModel* pModel) const;

	/**
	 *	Move index up one row in the list.
	 *	Index will not be moved past the start of the list.
	 *	@param index to be moved.
	 *	@param pModel to be used if index is invalid.
	 *	@return new position or
	 *		the first index in the model if nothing was selected or
	 *		the original index on error.
	 */
	Q_INVOKABLE QModelIndex getPreviousIndex(const QModelIndex& index, QAbstractItemModel* pModel) const;

	/**
	 *	Expand the current item if it is expandable or move to the next index.
	 *	@param index to be moved.
	 *	@param pModel to be used if index is invalid.
	 *	@return new position or
	 *		the first index in the model if nothing was selected or
	 *		the original index if the item was expanded or on error.
	 */
	Q_INVOKABLE QModelIndex getForwardIndex(const QModelIndex& index, QAbstractItemModel* pModel) const;

	/**
	 *	Collapse the current item if it is collapsible or move to the previous index.
	 *	@param index to be moved.
	 *	@param pModel to be used if index is invalid.
	 *	@return new position or
	 *		the first index in the model if nothing was selected or
	 *		the original index if the item was expanded or on error.
	 */
	Q_INVOKABLE QModelIndex getBackwardIndex(const QModelIndex& index, QAbstractItemModel* pModel) const;

	Q_INVOKABLE void expand(const QModelIndex& index);
	Q_INVOKABLE void collapse(const QModelIndex& index);
	Q_INVOKABLE void toggle(const QModelIndex& index);

private:
	struct Implementation;
	std::unique_ptr<Implementation> impl_;
};
} // end namespace wgt
#endif // TREE_EXTENSION_HPP
