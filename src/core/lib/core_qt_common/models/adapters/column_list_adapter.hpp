#ifndef COLUMN_LIST_ADAPTER_HPP
#define COLUMN_LIST_ADAPTER_HPP

#include "i_list_adapter.hpp"
#include "core_qt_common/qt_new_handler.hpp"

namespace wgt
{
/**
 *	Convert single-column Qt lists into multiple-column WGTF lists.
 *	Takes Qt list row indexes and map some of them to columns.
 *	
 *	E.g. Qt list models only have rows
 *	row +---+---+---+---+
 *	 0  |               |
 *	    +---+---+---+---+
 *	 1  |               |
 *	    +---+---+---+---+
 *	 2  |               |
 *	    +---+---+---+---+
 *	WGTF list models support columns
 *	column 0  1   2   3
 *	row +---+---+---+---+
 *	 0  |   |   |   |   |
 *	    +---+---+---+---+
 *	 1  |   |   |   |   |
 *	    +---+---+---+---+
 *	 2  |   |   |   |   |
 *	    +---+---+---+---+
 *	ColumnListAdapter adds extra "row indexes" to the Qt model.
 *	The extra "rows" are then laid out horizontally to form columns.
 *	Qt rows
 *	    +---+---+---+---+
 *	    | 0 | 1 | 2 | 3 |
 *	    +---+---+---+---+
 *	    | 4 | 5 | 6 | 7 |
 *	    +---+---+---+---+
 *	    | 8 | 9 |10 |11 |
 *	    +---+---+---+---+
 */
class ColumnListAdapter : public IListAdapter
{
	Q_OBJECT

	DECLARE_QT_MEMORY_HANDLER

public:
	ColumnListAdapter(const QModelIndex& row);
	virtual ~ColumnListAdapter();

	virtual QAbstractItemModel* model() const override;

	virtual QModelIndex adaptedIndex(int row,
	                                 int column,
	                                 const QModelIndex& parent) const override;
	virtual int rowCount(const QModelIndex& parent) const override;

private:
	void onParentDataChanged(const QModelIndex& topLeft,
	                         const QModelIndex& bottomRight,
	                         const QVector<int>& roles) override;
	void onParentLayoutChanged(const QList<QPersistentModelIndex>& parents,
	                           QAbstractItemModel::LayoutChangeHint hint) override;
	void onParentColumnsAboutToBeInserted(const QModelIndex& parent, int first, int last);
	void onParentColumnsInserted(const QModelIndex& parent, int first, int last);
	void onParentColumnsAboutToBeRemoved(const QModelIndex& parent, int first, int last);
	void onParentColumnsRemoved(const QModelIndex& parent, int first, int last);
	void onParentColumnsAboutToBeMoved(const QModelIndex& sourceParent,
	                                   int sourceFirst,
	                                   int sourceLast,
	                                   const QModelIndex& destinationParent,
	                                   int destinationColumn);
	void onParentColumnsMoved(const QModelIndex& sourceParent,
	                          int sourceFirst,
	                          int sourceLast,
	                          const QModelIndex& destinationParent,
	                          int destinationColumn);

private:
	const QAbstractItemModel* model_;
	QPersistentModelIndex row_;
	QPersistentModelIndex parent_;
};
} // end namespace wgt
#endif // COLUMN_LIST_ADAPTER_HPP
