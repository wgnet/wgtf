#ifndef QT_ABSTRACT_ITEM_MODEL_HPP
#define QT_ABSTRACT_ITEM_MODEL_HPP

#include <QAbstractItemModel>

#include <memory>

#include "qt_qlist_memory_fix.hpp"
SPECIALIZE_QLIST(QModelIndex)

namespace wgt
{
class AbstractItemModel;
class AbstractListModel;
class AbstractTreeModel;
class AbstractTableModel;

/** Adapter layer to allow AbstractItemModels or ExtendedModels to be used by Qt and QML views.*/
class QtAbstractItemModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	typedef AbstractItemModel SourceType;

	QtAbstractItemModel();
	virtual ~QtAbstractItemModel();

	/** Find a model index by its internal id.
	@ingroup qmlaccessible
	@param id Internal item id.
	@return A Qt model index. */
	Q_INVOKABLE virtual QModelIndex findIndex(QVariant id) const;

	/** Find a collection of model indexes by their internal ids.
	@ingroup qmlaccessible
	@param ids A Collection of Internal item ids.
	@return A Collection Qt model indexes. */
	Q_INVOKABLE virtual QModelIndexList findIndexes(QVariantList ids) const;

	/** Converts an item to an index.
	@ingroup qmlaccessible
	@param item ItemData pointer to convert.
	@return A Qt model index. */
	Q_INVOKABLE QModelIndex itemToIndex(QObject* item) const;

	/** Converts an index to an item.
	@ingroup qmlaccessible
	@param index Qt model index to convert.
	@return An ItemData pointer. */
	Q_INVOKABLE QObject* indexToItem(const QModelIndex& index) const;

	/** Converts a collection of indexes to a collection of items.
	@ingroup qmlaccessible
	@param indexes A collection of Qt model indexes to convert.
	@return A collection of ItemData pointers. */
	Q_INVOKABLE QList<QObject*> indexesToItems(const QModelIndexList& indexes) const;

	/** Returns the item at the given row, column, and parent position.
	@ingroup qmlaccessible
	@param row The row of the item.
	@param column The column of the item.
	@return The parent of the item. */
	Q_INVOKABLE QObject* item(int row, int column, QObject* parent) const;

	/** Returns the parent item for this child item.
	@ingroup qmlaccessible
	@param child The child ItemData pointer to use.
	@return The parent ItemData pointer for the child. */
	Q_INVOKABLE QObject* parent(QObject* child) const;

	/** Returns the row number from an item.
	@ingroup qmlaccessible
	@note A nullptr item implies the top level.
	@param item The ItemData pointer to use.
	@return Row number under the item. */
	Q_INVOKABLE int rowIndex(QObject* item) const;

	/** Returns the column number from an item.
	@ingroup qmlaccessible
	@note A nullptr item implies the top level.
	@param item The ItemData pointer to use.
	@return Column number under the item. */
	Q_INVOKABLE int columnIndex(QObject* item) const;

	/** Returns the number of rows under a parent item.
	@ingroup qmlaccessible
	@note A nullptr item implies the top level.
	@param parent The parent ItemData pointer to use.
	@return The number of rows. */
	Q_INVOKABLE int rowCount(QObject* parent) const;

	/** Returns the number of columns under a parent item.
	@ingroup qmlaccessible
	@note A nullptr item implies the top level.
	@param parent The parent ItemData pointer to use.
	@return The number of columns. */
	Q_INVOKABLE int columnCount(QObject* parent) const;

	/** Checks if this parent item has any children.
	@ingroup qmlaccessible
	@param parent The parent ItemData pointer to use.
	@return True if at least one child item exists. */
	Q_INVOKABLE bool hasChildren(QObject* parent) const;

	/** Inserts a new blank row at the specified position for the specified parent.
	@ingroup qmlaccessible
	@note A nullptr item implies the top level.
	@param row Row position to insert at.
	@param parent Parent to insert under.
	@return True if a row was inserted. */
	Q_INVOKABLE bool insertRow(int row, QObject* parent);

	/** Inserts a new blank column at the specified position for the specified parent.
	@ingroup qmlaccessible
	@note A nullptr item implies the top level.
	@param column Column position to insert at.
	@param parent Parent to insert under.
	@return True if a column was inserted. */
	Q_INVOKABLE bool insertColumn(int column, QObject* parent);

	/** Removes the row at the specified position for the specified parent.
	@ingroup qmlaccessible
	@note A nullptr item implies the top level.
	@param row Row position to remove.
	@param parent Parent to remove from.
	@return True if the row was removed. */
	Q_INVOKABLE bool removeRow(int row, QObject* parent);

	/** Removes the column at the specified position for the specified parent.
	@ingroup qmlaccessible
	@note A nullptr item implies the top level.
	@param column Column position to remove.
	@param parent Parent to remove from.
	@return True if the column was removed. */
	Q_INVOKABLE bool removeColumn(int column, QObject* parent);

	/** Relocates a row from one parent and position to a new parent and position.
	@ingroup qmlaccessible
	@note A nullptr item implies the top level.
	@param sourceParent The parent containing the row to move.
	@param sourceRow The row number to move.
	@param destinationParent The parent to which the row is moved.
	@param destinationChild The target row number for the row to move.
	@return True if the move was successful. */
	Q_INVOKABLE bool moveRow(QObject* sourceParent, int sourceRow, QObject* destinationParent, int destinationChild);

	// Explicitly declare virtual functions from QAbstractItemModel to prevent them from being hidden

	/** Returns the parent index of a child index.
	@note An invalid parent index implies the top level.
	@param child The child Qt model index.
	@return The parent Qt model index. */
	virtual QModelIndex parent(const QModelIndex& child) const override = 0;

	/** Returns the number of rows under a parent index.
	@note An invalid parent index implies the top level.
	@param parent The parent Qt model index.
	@return The number of rows. */
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override = 0;

	/** Returns the number of columns under a parent index.
	@note An invalid parent index implies the top level.
	@param parent The parent Qt model index.
	@return The number of columns. */
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override = 0;

	/** Checks whether there are any rows under a parent index.
	@note An invalid parent index implies the top level.
	@param parent The parent Qt model index.
	@return True if at least one row exists. */
	virtual bool hasChildren(const QModelIndex& parent = QModelIndex()) const override;

	Q_INVOKABLE Qt::ItemFlags flags(const QModelIndex& index) const override;

Q_SIGNALS:
	void modelChanged();
	void modelResetComplete();
	void layoutChangedComplete();

private:
	struct Impl;
	std::unique_ptr<Impl> impl_;
};

/** Provides QML invokable functions to iterate, insert and remove items by row.*/
class QtListModel : public QtAbstractItemModel
{
	Q_OBJECT

public:
	typedef AbstractListModel SourceType;

	QtListModel();

	/** Gets the item at a row position.
	@ingroup qmlaccessible
	@param row The row number for the item.
	@return The item at the given location. */
	Q_INVOKABLE QObject* item(int row) const;

	/** Gets number of rows in list.
	@ingroup qmlaccessible
	@return The number of rows. */
	Q_INVOKABLE int count() const;

	/** Adds a new row.
	Inserts the item *before* the given row.
	If row is rowCount(), the row is added to the end.
	@ingroup qmlaccessible
	@param row The row number for the new item.
	@return True on success. */
	Q_INVOKABLE bool insertItem(int row);

	/** Removes a row.
	@ingroup qmlaccessible
	@param row The row number of item to be removed.
	@return True on success. */
	Q_INVOKABLE bool removeItem(int row);

	/** Moves a row.
	@ingroup qmlaccessible
	@param sourceRow The row number of item to be moved.
	@param destinationRow The target row number to move to.
	@return True on success. */
	Q_INVOKABLE bool moveItem(int sourceRow, int destinationRow);
};

/** Provides QML invokable functions to iterate, insert and remove items by row and parent.*/
class QtTreeModel : public QtAbstractItemModel
{
	Q_OBJECT

public:
	typedef AbstractTreeModel SourceType;

	QtTreeModel();

	/** Gets the item at a row and parent object.
	@ingroup qmlaccessible
	@param row The row number for the item.
	@param column The item's parent object.
	@return The item at the given location. */
	Q_INVOKABLE QObject* item(int row, QObject* parent) const;

	/** Gets number of rows under parent.
	@ingroup qmlaccessible
	@return The number of child rows. */
	Q_INVOKABLE int count(QObject* parent) const;

	/** Adds a new row.
	Inserts the item *before* the given row, under a parent item.
	If row is count(), the row is added to the end.
	@ingroup qmlaccessible
	@note An null parent implies the top level of the model.
	@param row The row number for the new item.
	@param parent The parent item in the tree.
	@return True on success. */
	Q_INVOKABLE bool insertItem(int row, QObject* parent);

	/** Removes a row.
	Removes the item at the given row, under a parent item.
	@ingroup qmlaccessible
	@note An null parent implies the top level of the model.
	@param row The row number of item to be removed.
	@param parent The parent item in the tree.
	@return True on success. */
	Q_INVOKABLE bool removeItem(int row, QObject* parent);

	/** Moves a row.
	Moved a row from one parent and row number to another parent and row.
	@ingroup qmlaccessible
	@note An null parent implies the top level of the model.
	@param sourceParent The parent object of the item to be moved.
	@param sourceRow The row number of item to be moved.
	@param destinationParent The parent object to move to.
	@param destinationRow The target row number to move to.
	@return True on success. */
	Q_INVOKABLE bool moveItem(QObject* sourceParent, int sourceRow, QObject* destinationParent, int destinationRow);
};

/** Provides QML invokable functions to iterate, insert and remove items by row and column.*/
class QtTableModel : public QtAbstractItemModel
{
	Q_OBJECT

public:
	typedef AbstractTableModel SourceType;

	QtTableModel();

	/** Gets the item at a row and column position.
	@ingroup qmlaccessible
	@param row The row number for the item.
	@param column The column number for the item.
	@return The item at the given location. */
	Q_INVOKABLE QObject* item(int row, int column) const;

	/** Gets number of rows in the table.
	@ingroup qmlaccessible
	@return The number of rows. */
	Q_INVOKABLE int rowCount() const;

	/** Gets number of columns in the table.
	@ingroup qmlaccessible
	@return The number of columns. */
	Q_INVOKABLE int columnCount() const;

	/** Adds a new row.
	Inserts the item *before* the given row.
	If row is rowCount(), the row is added to the end.
	@ingroup qmlaccessible
	@param row The row number for the new item.
	@return True on success. */
	Q_INVOKABLE bool insertRow(int row);

	/** Adds a new column.
	Inserts the item *before* the given column.
	If column is columnCount(), the column is added to the end.
	@ingroup qmlaccessible
	@param The column number for the new item.
	@return True on success. */
	Q_INVOKABLE bool insertColumn(int column);

	/** Removes a row.
	@ingroup qmlaccessible
	@param row The row number of item to be removed.
	@return True on success. */
	Q_INVOKABLE bool removeRow(int row);

	/** Removes a column.
	@ingroup qmlaccessible
	@param column The column number of item to be removed.
	@return True on success. */
	Q_INVOKABLE bool removeColumn(int column);

	/** Moves a row.
	@ingroup qmlaccessible
	@param sourceRow The row number of item to be moved.
	@param destinationRow The target row number to move to.
	@return True on success. */
	Q_INVOKABLE bool moveRow(int sourceRow, int destinationRow);

private:
	virtual int rowCount(const QModelIndex& parent) const override = 0;
	virtual int columnCount(const QModelIndex& parent) const override = 0;
};
} // end namespace wgt
#endif // QT_ABSTRACT_ITEM_MODEL_HPP
