#ifndef QT_ITEM_MODEL_HPP
#define QT_ITEM_MODEL_HPP

#include "core_data_model_cmds/interfaces/i_item_model_controller.hpp"
#include "core_dependency_system/di_ref.hpp"
#include "qt_abstract_item_model.hpp"
#include "core_qt_common/qt_new_handler.hpp"
#include <memory>
#include "role_provider.hpp"

namespace wgt
{
class AbstractItemModel;
class AbstractListModel;
class AbstractTreeModel;
class AbstractTableModel;
class IComponentContext;

/** Adapter layer to allow any AbstractItemModel to be used by Qt and QML views.*/
class QtItemModel : public QtAbstractItemModel, public RoleProvider
{
	Q_OBJECT
	DECLARE_QT_MEMORY_HANDLER
public:
	QtItemModel(IComponentContext& context, AbstractItemModel& source);
	virtual ~QtItemModel();

	/** Gets AbstractItemModel that is being adapted to be used by Qt.
	@return A constant reference to the attached AbstractItemModel. */
	const AbstractItemModel& source() const;

	/** Gets AbstractItemModel that is being adapted to be used by Qt.
	@return A reference to the attached AbstractItemModel. */
	AbstractItemModel& source();

	/** Converts a location in the model into an index.
	@note An invalid index implies the top level of the model.
	@param row The row number for the index.
	@param column The column number for the index.
	@param parent The index this index falls under.
	@return A model index containing the location information. */
	QModelIndex index(int row, int column, const QModelIndex& parent) const override;

	/** Finds the parent index for an index.
	@param child The index to find the parent for.
	@return The index's parent index. */
	QModelIndex parent(const QModelIndex& child) const override;

	/** Gets number of rows in the model, relating to a parent.
	@note An invalid index implies the top level of the model.
	@param parent The parent index.
	@return The number of rows. */
	int rowCount(const QModelIndex& parent) const override;

	/** Gets number of columns in the model, relating to a parent.
	@note An invalid index implies the top level of the model.
	@param parent The parent index.
	@return The number of columns. */
	int columnCount(const QModelIndex& parent) const override;

	/** Determines if there are any indices under a parent index.
	@note An invalid index implies the top level of the model.
	@param parent The parent index to check under.
	@return The number of indices under the parent index. */
	bool hasChildren(const QModelIndex& parent) const override;

	/** Gets role data from an item at an index position.
	This converts the index and role to the format of the source model, calls
	the source model's getData, converts the value to a Qt compatible value,
	and returns that converted value.
	@param index The location of the item.
	@param role The role identifier of the value to get.
	@return The value of the role. */
	QVariant data(const QModelIndex& index, int role) const override;

	/** Changes role data for an item at an index position.
	This converts the index, value, and role to the format of the source model, calls
	the source model's setData, and returns whether if was successful or not.
	@param index The location of the item.
	@param value The new value of the role.
	@param role The role identifier of the value to get.
	@return True if successful. */
	bool setData(const QModelIndex& index, const QVariant& value, int role) override;

	/** Gets role header data at a position in the header.
	This converts the role to the source model's format, calls the source model's getHeaderData,
	converts the value to a Qt compatible value, and returns that converted value.
	@param section The location of the item, either column or row depending on orientation.
	@param orientation The layout of the header, either horizontal or vertical.
	@param role The role identifier of the value to get.
	@return The value of the role. */
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

	/** Changes role header data at a position in the header.
	This converts the role and value to the source model's format,
	calls the source model's setHeaderData,	and returns whether it was successful or not.
	@param section The location of the item, either column or row depending on orientation.
	@param orientation The layout of the header, either horizontal or vertical.
	@param value The new value of the role.
	@param role The role identifier of the value to get.
	@return True if successful. */
	bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role) override;

	/** Inserts new blank rows at the specified position for the specified parent.
	@note If row is rowCount(), the rows are added to the end.
	@note An invalid index implies the top level of the model.
	@param row Row position to insert at.
	@param count Number of rows to insert.
	@param parent Parent index to insert under.
	@return True if a row was inserted. */
	bool insertRows(int row, int count, const QModelIndex& parent) override;

	/** Inserts new columns at the specified position for the specified parent.
	@note If column is columnCount(), the column is added to the end.
	@note An invalid index implies the top level of the model.
	@param column Column position to insert at.
	@param count Number of columns to insert.
	@param parent Parent index to insert under.
	@return True if a column was inserted. */
	bool insertColumns(int column, int count, const QModelIndex& parent) override;

	/** Removes rows at the specified position for the specified parent.
	@note An invalid index implies the top level of the model.
	@param row Row position to remove.
	@param count Number of rows to remove.
	@param parent Parent index to remove from.
	@return True if a row was removed. */
	bool removeRows(int row, int count, const QModelIndex& parent) override;

	/** Removes columns at the specified position for the specified parent.
	@note An invalid index implies the top level of the model.
	@param column Column position to remove.
	@param count Number of columns to remove.
	@param parent Parent index to remove from.
	@return True if a column was removed. */
	bool removeColumns(int column, int count, const QModelIndex& parent) override;

	/** Moves a number of rows from one position and parent
	to another position and parent.
	@note An invalid index implies the top level of the model.
	@param sourceParent The parent index containing the rows to move.
	@param sourceRow The starting row number to move.
	@param count The amount of rows to move.
	@param destinationParent The parent index to which the rows are moved.
	@param destinationChild The target row number for the first row to move.
	@return True if the rows were moved. */
	bool moveRows(const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent,
	              int destinationChild) override;

	virtual QStringList mimeTypes() const override;
	virtual QMimeData* mimeData(const QModelIndexList& indexes) const override;
	virtual bool canDropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column,
	                             const QModelIndex& parent) const override;
	virtual bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column,
	                          const QModelIndex& parent) override;

	/** Returns the supported flags for this model.
	The supported flags include the default flags, as well the the following:
	For valid indices:
	    Qt::ItemIsEnabled
	    Qt::ItemIsSelectable
	    Qt::ItemIsEditable
	    Qt::ItemIsDragEnabled
	    Qt::ItemIsDropEnabled
	For invalid indices:
	    Qt::ItemIsDropEnabled
	More information about these can be found here: http://doc.qt.io/qt-5/qt.html#ItemFlag-enum
	This field can contain multiple bit flags.
	@ingroup qmlaccessible
	@param index The index to which the flags apply to.
	@return The combination of flags. */
	Q_INVOKABLE Qt::ItemFlags flags(const QModelIndex& index) const override;

	/** Returns a map of supported roles to be used in QML.
	These are mapped from role identifier to role name.
	@return The roles map. */
	QHash<int, QByteArray> roleNames() const override;

	bool encodeRole(ItemRole::Id roleId, int& o_Role) const override;
	bool decodeRole(int role, ItemRole::Id& o_RoleId) const override;

protected:
	DIRef<IItemModelController> itemModelController_;

private:
	struct Impl;
	std::unique_ptr<Impl> impl_;
};

/** Provides QML invokable functions to iterate, insert and remove items by row.*/
class QtListModel : public QtItemModel
{
	Q_OBJECT

public:
	QtListModel(IComponentContext& context, AbstractListModel& source);

	/** Gets AbstractListModel that is being adapted to be used by Qt.
	@return A constant reference to the attached AbstractListModel. */
	const AbstractListModel& source() const;

	/** Gets AbstractListModel that is being adapted to be used by Qt.
	@return A reference to the attached AbstractListModel. */
	AbstractListModel& source();

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
class QtTreeModel : public QtItemModel
{
	Q_OBJECT

public:
	QtTreeModel(IComponentContext& context, AbstractTreeModel& source);

	/** Gets AbstractTreeModel that is being adapted to be used by Qt.
	@return A constant reference to the attached AbstractTreeModel. */
	const AbstractTreeModel& source() const;

	/** Gets AbstractTreeModel that is being adapted to be used by Qt.
	@return A reference to the attached AbstractTreeModel. */
	AbstractTreeModel& source();

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
class QtTableModel : public QtItemModel
{
	Q_OBJECT

public:
	QtTableModel(IComponentContext& context, AbstractTableModel& source);

	/** Gets AbstractTableModel that is being adapted to be used by Qt.
	@return A constant reference to the attached AbstractTableModel. */
	const AbstractTableModel& source() const;

	/** Gets AbstractTableModel that is being adapted to be used by Qt.
	@return A reference to the attached AbstractTableModel. */
	AbstractTableModel& source();

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
	using QtItemModel::rowCount;
	using QtItemModel::columnCount;
};
} // end namespace wgt
#endif // QT_ITEM_MODEL_HPP
