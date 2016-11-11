#ifndef QT_ABSTRACT_ITEM_MODEL_HPP
#define QT_ABSTRACT_ITEM_MODEL_HPP

#include <QAbstractItemModel>

#include <memory>

namespace wgt
{
/** Adapter layer to allow AbstractItemModels or ExtendedModels to be used by Qt and QML views.*/
class QtAbstractItemModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	QtAbstractItemModel();
	virtual ~QtAbstractItemModel();

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
	virtual QModelIndex parent(const QModelIndex& child) const = 0;

	/** Returns the number of rows under a parent index.
	@note An invalid parent index implies the top level.
	@param parent The parent Qt model index.
	@return The number of rows. */
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const = 0;

	/** Returns the number of columns under a parent index.
	@note An invalid parent index implies the top level.
	@param parent The parent Qt model index.
	@return The number of columns. */
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const = 0;

	/** Checks whether there are any rows under a parent index.
	@note An invalid parent index implies the top level.
	@param parent The parent Qt model index.
	@return True if at least one row exists. */
	virtual bool hasChildren(const QModelIndex& parent = QModelIndex()) const;

private:
	struct Impl;
	std::unique_ptr<Impl> impl_;
};
} // end namespace wgt
#endif // QT_ABSTRACT_ITEM_MODEL_HPP
