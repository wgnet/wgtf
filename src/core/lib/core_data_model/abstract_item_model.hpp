#ifndef ABSTRACT_ITEM_MODEL_HPP
#define ABSTRACT_ITEM_MODEL_HPP

#include "abstract_item.hpp"
#include <map>
namespace wgt
{
typedef std::map<std::string, std::vector<char>> MimeData;

enum DropAction
{
	MoveAction,
	CopyAction,
	InvalidAction
};

/** Base class for all types of item based data models. */
class AbstractItemModel : public AbstractItem
{
public:
	/** Structure for indexing an item model.
	It is a combination of an optional parent item, a row number, and a column number. */
	struct ItemIndex
	{
		ItemIndex(int row = -1, int column = -1, const AbstractItem* parent = nullptr)
		    : row_(row), column_(column), parent_(parent)
		{
		}

		/** Checks whether the index has valid data.
		An index is valid if neither the row, nor the column are negative. */
		bool isValid() const
		{
			return row_ >= 0 && column_ >= 0;
		}

		/** Checks equality of two indices.
		If two indices are both invalid, they are seen as equal, regardless of their actual values. */
		bool operator==(const ItemIndex& other) const
		{
			if (!isValid() && !other.isValid())
			{
				return true;
			}

			return row_ == other.row_ && column_ == other.column_ && parent_ == other.parent_;
		}

		/** Checks inequality of two indices.
		If two indices are both invalid, they are seen as equal, regardless of their actual values. */
		bool operator!=(const ItemIndex& other) const
		{
			return !this->operator==(other);
		}

		int row_;
		int column_;
		const AbstractItem* parent_;
	};

	typedef void VoidSignature(void);
	typedef std::function<VoidSignature> VoidCallback;

	/** Data changed at *row*, with the given role and new value.
	@note newValue is always the new value, for both pre- and post- callbacks. */
	typedef void DataSignature(const ItemIndex& index, ItemRole::Id role, const Variant& newValue);
	typedef std::function<DataSignature> DataCallback;

	/** Insert/remove into *parentIndex* from *startPos* to *startPos + count*.
	@param parentIndex item inside which to do the insertion/removal.
	@param startPos first row or column of insertion/removal under the parent.
	@param count number of rows or columns after startPos. */
	typedef void RangeSignature(const ItemIndex& parentIndex, int startPos, int count);
	typedef std::function<RangeSignature> RangeCallback;

	/** Move items under *sourceParentIndex* from *sourceFirst* to *sourceLast*
	into *destinationParentIndex* *destinationFirst*.
	@param sourceParentIndex item inside which to do the removal.
	@param sourceFirst first row or column of removal under the parent.
	@param sourceLast last row or column of removal under the parent.
	@param destinationParentIndex item inside which to do the insertion.
	@param destinationFirst first row or column for insertion under the parent. */
	typedef void MoveSignature(const ItemIndex& sourceParentIndex, int sourceFirst, int sourceLast,
	                           const ItemIndex& destinationParentIndex, int destinationFirst);
	typedef std::function<MoveSignature> MoveCallback;

	virtual ~AbstractItemModel()
	{
	}

	/** Get the AbstractItem pointer at the given ItemIndex.
	@note item pointers may be deleted by removeRows() or removeColumns().
	@param index The index of the item.
	@return The AbstractItem pointer if found, otherwise nullptr. */
	virtual AbstractItem* item(const ItemIndex& index) const = 0;

	/** Finds the index of an AbstractItem pointer.
	@param item The AbstractItem pointer to find.
	@param o_Index The index to return. */
	virtual void index(const AbstractItem* item, ItemIndex& o_Index) const = 0;

	/** Returns the number of rows under an item.
	@note A nullptr item implies the top level.
	@param item The AbstractItem pointer whose rows are to be counted.
	@return The number of rows. */
	virtual int rowCount(const AbstractItem* item) const = 0;

	/** Returns the number of columns under an item.
	@note A nullptr item implies the top level.
	@param item The AbstractItem pointer whose columns are to be counted.
	@return The number of columns. */
	virtual int columnCount(const AbstractItem* item) const = 0;

	/** Checks if any children exists under an item.
	@note A nullptr item implies the top level.
	@param item The AbstractItem pointer to be checked for children.
	@return True if at least one child was found. */
	virtual bool hasChildren(const AbstractItem* item) const
	{
		return rowCount(item) > 0;
	}

	/** Inserts new rows with default values at the specified position for the specified parent.
	@note A nullptr parent implies the top level.
	@note If successful, the pre and post signals need to be fired.
	The following connect methods need to be implemented for this to work:
	connectPreRowsInserted, connectPostRowsInserted
	@post Item pointers previously obtained using item() are guaranteed not to
	have been deleted.
	@param row Row position to insert at.
	@param count Number of rows to insert.
	@param parent Parent to insert under.
	@return True if a row was inserted. */
	virtual bool insertRows(int row, int count, const AbstractItem* parent)
	{
		return false;
	}

	/** Inserts new columns at the specified position for the specified parent.
	@note A nullptr parent implies the top level.
	@note If successful, the pre and post signals need to be fired.
	The following connect methods need to be implemented for this to work:
	connectPreColumnsInserted, connectPostColumnsInserted
	@post Item pointers previously obtained using item() are guaranteed not to
	have been deleted.
	@param column Column position to insert at.
	@param count Number of columns to insert.
	@param parent Parent to insert under.
	@return True if a column was inserted. */
	virtual bool insertColumns(int column, int count, const AbstractItem* parent)
	{
		return false;
	}

	/** Removes rows at the specified position for the specified parent.
	@note A nullptr parent implies the top level.
	@note If successful, the pre and post signals need to be fired.
	The following connect methods need to be implemented for this to work:
	connectPreRowsReoved, connectPostRowsRemoved
	@post Item pointers previously obtained using item() in the removed range
	have their behaviour undefined.
	Item pointers outside the range are guaranteed not to have been deleted.
	@param row Row position to remove.
	@param count Number of rows to remove.
	@param parent Parent to remove from.
	@return True if a row was removed. */
	virtual bool removeRows(int row, int count, const AbstractItem* parent)
	{
		return false;
	}

	/** Removes columns at the specified position for the specified parent.
	@note A nullptr parent implies the top level.
	@note If successful, the pre and post signals need to be fired.
	The following connect methods need to be implemented for this to work:
	connectPreColumnsRemoved, connectPostColumnRemoved
	@post Item pointers previously obtained using item() in the removed range
	have their behaviour undefined.
	Item pointers outside the range are guaranteed not to have been deleted.
	@param column Column position to remove.
	@param count Number of columns to remove.
	@param parent Parent to remove from.
	@return True if a column was removed. */
	virtual bool removeColumns(int column, int count, const AbstractItem* parent)
	{
		return false;
	}

	/** Moves a number of rows from one position and parent
	to another position and parent.
	@note A nullptr parent implies the top level.
	@note If successful, the pre and post signals need to be fired.
	The following connect methods need to be implemented for this to work:
	connectPreRowsMoved, connectPostRowsMoved
	@post Item pointers previously obtained using item() are guaranteed not to
	have been deleted.
	@param sourceParent The parent containing the rows to move.
	@param sourceRow The starting row number to move.
	@param count The amount of rows to move.
	@param destinationParent The parent to which the rows are moved.
	@param destinationChild The target row number for the first row to move.
	@return True if the rows were moved. */
	virtual bool moveRows(const AbstractItem* sourceParent, int sourceRow, int count,
	                      const AbstractItem* destinationParent, int destinationChild)
	{
		return false;
	}

	/** Returns a mime data map of format to data.
	@note mime data is described here: https://www.iana.org/assignments/media-types/media-types.xhtml
	@note The default implementation does nothing.
	@param indices All indices to include in the data map.
	@return The mime data map of formats to data. */
	virtual MimeData mimeData(std::vector<ItemIndex>& indices)
	{
		return MimeData();
	}

	/** Returns a list of supported types for drag and drop.
	@note mime data is described here: https://www.iana.org/assignments/media-types/media-types.xhtml
	@note The default implementation does nothing.
	@return The list of supported types. */
	virtual std::vector<std::string> mimeTypes() const
	{
		return std::vector<std::string>();
	}

	virtual bool canDropMimeData(const MimeData& mimeData, DropAction action, const ItemIndex& index) const
	{
		return false;
	}

	virtual bool dropMimeData(const MimeData& mimeData, DropAction action, const ItemIndex& index)
	{
		return false;
	}

	/** Returns a list of roles used by this model.
	Roles are a way of providing properties without changing this interface.
	@return A vector of string role names. */
	virtual std::vector<std::string> roles() const
	{
		return std::vector<std::string>();
	}

	virtual Connection connectPreModelReset(VoidCallback callback)
	{
		return Connection();
	}
	virtual Connection connectPostModelReset(VoidCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call before an item's data changed.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPreItemDataChanged(DataCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call after an item's data changed.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPostItemDataChanged(DataCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call before rows are inserted.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPreRowsInserted(RangeCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call after rows were inserted.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPostRowsInserted(RangeCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call before rows are removed.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPreRowsRemoved(RangeCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call after rows were removed.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPostRowsRemoved(RangeCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call before columns are inserted.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPreColumnsInserted(RangeCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call after columns were inserted.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPostColumnsInserted(RangeCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call before columns are removed.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPreColumnsRemoved(RangeCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call after columns were removed.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPostColumnsRemoved(RangeCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call before rows are moved.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPreRowsMoved(MoveCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call after rows are moved.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPostRowsMoved(MoveCallback callback)
	{
		return Connection();
	}
};

/** Provide models with items arranged one after the other; a list.
Items in rows are all connected, cannot have different items in different columns. */
class AbstractListModel : public AbstractItemModel
{
public:
	/** Data changed at *row*, with the given column, role and new value.
	@note newValue is always the new value, for both pre- and post- callbacks. */
	typedef void DataSignature(int row, int column, ItemRole::Id role, const Variant& newValue);
	typedef std::function<DataSignature> DataCallback;

	/** Insert/remove from *startRow* to *startRow + count*.
	@param startRow first row of insertion/removal.
	@param count number of rows after startRow. */
	typedef void RangeSignature(int startRow, int count);
	typedef std::function<RangeSignature> RangeCallback;

	/** Move items from *sourceFirst* to *sourceLast* into *destinationFirst*.
	@param sourceFirst first row or column of removal under the parent.
	@param sourceLast last row or column of removal under the parent.
	@param destinationFirst first row or column for insertion under the parent. */
	typedef void MoveSignature(int sourceFirst, int sourceLast, int destinationFirst);
	typedef std::function<MoveSignature> MoveCallback;

	virtual ~AbstractListModel()
	{
	}

	/** Get the AbstractItem pointer at the given row.
	@note This is a specialization of the inherited method, removing irrelevant parameters.
	@param index The index of the item.
	@return The AbstractItem pointer if found, otherwise nullptr. */
	virtual AbstractItem* item(int row) const = 0;

	/** Finds the row of an AbstractItem pointer.
	@param item The AbstractItem pointer to find.
	@return The row number to return. */
	virtual int index(const AbstractItem* item) const = 0;

	/** Returns the number of rows.
	@return The number of rows. */
	virtual int rowCount() const = 0;

	/** Returns the number of columns.
	@return The number of columns. */
	virtual int columnCount() const = 0;

	/** Inserts new rows with default values at the specified position.
	@note If successful, the pre and post signals need to be fired.
	@param row Row position to insert at.
	@param count Number of rows to insert.
	@return True if a row was inserted. */
	virtual bool insertRows(int row, int count)
	{
		return false;
	}

	/** Inserts new columns at the specified position.
	@note If successful, the pre and post signals need to be fired.
	@param column Column position to insert at.
	@param count Number of columns to insert.
	@return True if a column was inserted. */
	virtual bool insertColumns(int column, int count)
	{
		return false;
	}

	/** Removes rows at the specified position.
	@note If successful, the pre and post signals need to be fired.
	@param row Row position to remove.
	@param count Number of rows to remove.
	@return True if a row was removed. */
	virtual bool removeRows(int row, int count)
	{
		return false;
	}

	/** Removes columns at the specified position.
	@note If successful, the pre and post signals need to be fired.
	@param column Column position to remove.
	@param count Number of columns to remove.
	@return True if a column was removed. */
	virtual bool removeColumns(int column, int count)
	{
		return false;
	}

	/** Moves a number of rows from one position to another.
	@note If successful, the pre and post signals need to be fired.
	@param sourceRow The starting row number to move.
	@param count The amount of rows to move.
	@param destinationChild The target row number for the first row to move.
	@return True if the rows were moved. */
	virtual bool moveRows(int sourceRow, int count, int destinationRow)
	{
		return false;
	}

	/** Adds a callback to call before an item's data changed.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPreItemDataChanged(DataCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call after an item's data changed.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPostItemDataChanged(DataCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call before rows are inserted.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPreRowsInserted(RangeCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call after rows were inserted.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPostRowsInserted(RangeCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call before rows are removed.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPreRowsRemoved(RangeCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call after rows were removed.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPostRowsRemoved(RangeCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call before rows are moved.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPreRowsMoved(MoveCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call after rows are moved.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPostRowsMoved(MoveCallback callback)
	{
		return Connection();
	}

private:
	// Override functions using base class ItemIndex and hide them
	// Users can use public overloads with current class' ItemIndex type

	AbstractItem* item(const AbstractItemModel::ItemIndex& index) const override
	{
		if (!index.isValid())
		{
			return nullptr;
		}

		if (index.parent_ != nullptr)
		{
			return nullptr;
		}

		return item(index.row_);
	}

	void index(const AbstractItem* item, AbstractItemModel::ItemIndex& o_Index) const override
	{
		int row = index(item);

		o_Index.row_ = row;
		o_Index.column_ = 0;
		o_Index.parent_ = nullptr;
	}

	int rowCount(const AbstractItem* item) const override
	{
		if (item != nullptr)
		{
			return 0;
		}

		return rowCount();
	}

	int columnCount(const AbstractItem* item) const override
	{
		return columnCount();
	}

	bool insertRows(int row, int count, const AbstractItem* parent) override
	{
		if (parent != nullptr)
		{
			return false;
		}

		return insertRows(row, count);
	}

	bool insertColumns(int column, int count, const AbstractItem* parent) override
	{
		return insertColumns(column, count);
	}

	bool removeRows(int row, int count, const AbstractItem* parent) override
	{
		if (parent != nullptr)
		{
			return false;
		}

		return removeRows(row, count);
	}

	bool removeColumns(int column, int count, const AbstractItem* parent) override
	{
		return removeColumns(column, count);
	}

	bool moveRows(const AbstractItem* sourceParent, int sourceRow, int count, const AbstractItem* destinationParent,
	              int destinationChild) override
	{
		if (sourceParent != nullptr)
		{
			return false;
		}
		if (destinationParent != nullptr)
		{
			return false;
		}

		return moveRows(sourceRow, count, destinationChild);
	}

	Connection connectPreItemDataChanged(AbstractItemModel::DataCallback callback) override
	{
		return connectPreItemDataChanged(
		(DataCallback)[=](int row, int column, ItemRole::Id role, const Variant& value) {
			callback(AbstractItemModel::ItemIndex(row, column, nullptr), role, value);
		});
	}

	Connection connectPostItemDataChanged(AbstractItemModel::DataCallback callback) override
	{
		return connectPostItemDataChanged(
		(DataCallback)[=](int row, int column, ItemRole::Id role, const Variant& value) {
			callback(AbstractItemModel::ItemIndex(row, column, nullptr), role, value);
		});
	}

	Connection connectPreRowsInserted(AbstractItemModel::RangeCallback callback) override
	{
		return connectPreRowsInserted(
		(RangeCallback)[=](int pos, int count) { callback(AbstractItemModel::ItemIndex(), pos, count); });
	}

	Connection connectPostRowsInserted(AbstractItemModel::RangeCallback callback) override
	{
		return connectPostRowsInserted(
		(RangeCallback)[=](int pos, int count) { callback(AbstractItemModel::ItemIndex(), pos, count); });
	}

	Connection connectPreRowsRemoved(AbstractItemModel::RangeCallback callback) override
	{
		return connectPreRowsRemoved(
		(RangeCallback)[=](int pos, int count) { callback(AbstractItemModel::ItemIndex(), pos, count); });
	}

	Connection connectPostRowsRemoved(AbstractItemModel::RangeCallback callback) override
	{
		return connectPostRowsRemoved(
		(RangeCallback)[=](int pos, int count) { callback(AbstractItemModel::ItemIndex(), pos, count); });
	}

	Connection connectPreRowsMoved(AbstractItemModel::MoveCallback callback) override
	{
		return connectPreRowsMoved((MoveCallback)[=](int sourceFirst, int sourceLast, int destinationFirst) {
			callback(AbstractItemModel::ItemIndex(), sourceFirst, sourceLast, AbstractItemModel::ItemIndex(),
			         destinationFirst);
		});
	}

	Connection connectPostRowsMoved(AbstractItemModel::MoveCallback callback) override
	{
		return connectPostRowsMoved((MoveCallback)[=](int sourceFirst, int sourceLast, int destinationFirst) {
			callback(AbstractItemModel::ItemIndex(), sourceFirst, sourceLast, AbstractItemModel::ItemIndex(),
			         destinationFirst);
		});
	}
};

/** Provide models with items arranged in a hierarchy; a tree.
Items in rows are all connected, cannot have different items in different columns. */
class AbstractTreeModel : public AbstractItemModel
{
public:
	/** Structure for indexing a tree model.
	It is a combination of an optional parent item and a row number. */
	struct ItemIndex
	{
		ItemIndex(int row = -1, const AbstractItem* parent = nullptr) : row_(row), parent_(parent)
		{
		}

		/** Checks whether the index has valid data.
		An index is valid as long as the row is not negative. */
		bool isValid() const
		{
			return row_ >= 0;
		}

		/** Checks equality of two indices.
		If two indices are both invalid, they are seen as equal, regardless of their actual values. */
		bool operator==(const ItemIndex& other) const
		{
			if (!isValid() && !other.isValid())
			{
				return true;
			}

			return row_ == other.row_ && parent_ == other.parent_;
		}

		/** Checks inequality of two indices.
		If two indices are both invalid, they are seen as equal, regardless of their actual values. */
		bool operator!=(const ItemIndex& other) const
		{
			return !this->operator==(other);
		}

		int row_;
		const AbstractItem* parent_;
	};

	/** Data changed at *index*, with the given column, role and new value.
	@note value is always the new value, for both pre- and post- callbacks. */
	typedef void DataSignature(const ItemIndex& index, int column, ItemRole::Id role, const Variant& value);
	typedef std::function<DataSignature> DataCallback;

	/** Insert/remove into the item at *parentIndex* from *startRow* to *startRow + count*.
	@param parentIndex item inside which to do the insertion/removal.
	@param startRow first row of insertion/removal under the parent.
	@param count number of rows after startRow. */
	typedef void RangeSignature(const ItemIndex& parentIndex, int startRow, int count);
	typedef std::function<RangeSignature> RangeCallback;

	/** Move items under *sourceParentIndex* from *sourceFirst* to *sourceLast*
	into *destinationParentIndex* *destinationRow*.
	@param sourceParentIndex item inside which to do the removal.
	@param sourceFirst first row or column of removal under the parent.
	@param sourceLast last row or column of removal under the parent.
	@param destinationParentIndex item inside which to do the insertion.
	@param destinationRow first row or column for insertion under the parent. */
	typedef void MoveSignature(const ItemIndex& sourceParentIndex, int sourceFirst, int sourceLast,
	                           const ItemIndex& destinationParentIndex, int destinationRow);
	typedef std::function<MoveSignature> MoveCallback;

	virtual ~AbstractTreeModel()
	{
	}

	/** Get the AbstractItem pointer at the given ItemIndex.
	@note This is a specialization of the inherited method, removing irrelevant parameters.
	@param index The index of the item.
	@return The AbstractItem pointer if found, otherwise nullptr. */
	virtual AbstractItem* item(const ItemIndex& index) const = 0;

	/** Finds the index of an AbstractItem pointer.
	@param item The AbstractItem pointer to find.
	@return The index to return. */
	virtual ItemIndex index(const AbstractItem* item) const = 0;

	/** Returns the number of rows under an item.
	@note A nullptr item implies the top level.
	@param item The AbstractItem pointer whose rows are to be counted.
	@return The number of rows. */
	virtual int rowCount(const AbstractItem* item) const override = 0;

	/** Returns the number of columns.
	@return The number of columns. */
	virtual int columnCount() const = 0;

	/** Inserts new columns at the specified position.
	@note If successful, the pre and post signals need to be fired.
	@param column Column position to insert at.
	@param count Number of columns to insert.
	@return True if a column was inserted. */
	virtual bool insertColumns(int column, int count)
	{
		return false;
	}

	/** Removes columns at the specified position.
	@note If successful, the pre and post signals need to be fired.
	@param column Column position to remove.
	@param count Number of columns to remove.
	@return True if a column was removed. */
	virtual bool removeColumns(int column, int count)
	{
		return false;
	}

	/** Moves a number of rows from one position to another.
	@note If successful, the pre and post signals need to be fired.
	@param sourceRow The starting row number to move.
	@param count The amount of rows to move.
	@param destinationChild The target row number for the first row to move.
	@return True if the rows were moved. */
	// TODO Include parents: NGT-2820
	virtual bool moveRows(int sourceRow, int count, int destinationRow)
	{
		return false;
	}

	/** Adds a callback to call before an item's data changed.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPreItemDataChanged(DataCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call after an item's data changed.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPostItemDataChanged(DataCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call before rows are inserted.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPreRowsInserted(RangeCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call after rows were inserted.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPostRowsInserted(RangeCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call before rows are removed.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPreRowsRemoved(RangeCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call after rows were removed.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPostRowsRemoved(RangeCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call before rows are moved.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPreRowsMoved(MoveCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call after rows are moved.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPostRowsMoved(MoveCallback callback)
	{
		return Connection();
	}

private:
	// Override functions using base class ItemIndex and hide them
	// Users can use public overloads with current class' ItemIndex type

	AbstractItem* item(const AbstractItemModel::ItemIndex& index) const override
	{
		if (!index.isValid())
		{
			return nullptr;
		}

		return item(ItemIndex(index.row_, index.parent_));
	}

	void index(const AbstractItem* item, AbstractItemModel::ItemIndex& o_Index) const override
	{
		ItemIndex index = this->index(item);

		o_Index.row_ = index.row_;
		o_Index.column_ = 0;
		o_Index.parent_ = index.parent_;
	}

	int columnCount(const AbstractItem* item) const override
	{
		return columnCount();
	}

	bool insertColumns(int column, int count, const AbstractItem* parent) override
	{
		return insertColumns(column, count);
	}

	bool removeColumns(int column, int count, const AbstractItem* parent) override
	{
		return removeColumns(column, count);
	}

	Connection connectPreItemDataChanged(AbstractItemModel::DataCallback callback) override
	{
		return connectPreItemDataChanged(
		(DataCallback)[=](const ItemIndex& index, int column, ItemRole::Id role, const Variant& value) {
			callback(AbstractItemModel::ItemIndex(index.row_, column, index.parent_), role, value);
		});
	}

	Connection connectPostItemDataChanged(AbstractItemModel::DataCallback callback) override
	{
		return connectPostItemDataChanged(
		(DataCallback)[=](const ItemIndex& index, int column, ItemRole::Id role, const Variant& value) {
			callback(AbstractItemModel::ItemIndex(index.row_, column, index.parent_), role, value);
		});
	}

	Connection connectPreRowsInserted(AbstractItemModel::RangeCallback callback) override
	{
		return connectPreRowsInserted((RangeCallback)[=](const ItemIndex& index, int pos, int count) {
			callback(AbstractItemModel::ItemIndex(index.row_, 0, index.parent_), pos, count);
		});
	}

	Connection connectPostRowsInserted(AbstractItemModel::RangeCallback callback) override
	{
		return connectPostRowsInserted((RangeCallback)[=](const ItemIndex& index, int pos, int count) {
			callback(AbstractItemModel::ItemIndex(index.row_, 0, index.parent_), pos, count);
		});
	}

	Connection connectPreRowsRemoved(AbstractItemModel::RangeCallback callback) override
	{
		return connectPreRowsRemoved((RangeCallback)[=](const ItemIndex& index, int pos, int count) {
			callback(AbstractItemModel::ItemIndex(index.row_, 0, index.parent_), pos, count);
		});
	}

	Connection connectPostRowsRemoved(AbstractItemModel::RangeCallback callback) override
	{
		return connectPostRowsRemoved((RangeCallback)[=](const ItemIndex& index, int pos, int count) {
			callback(AbstractItemModel::ItemIndex(index.row_, 0, index.parent_), pos, count);
		});
	}

	Connection connectPreRowsMoved(AbstractItemModel::MoveCallback callback) override
	{
		return connectPreRowsMoved((MoveCallback)[=](const ItemIndex& sourceParent, int sourceFirst, int sourceLast,
		                                             const ItemIndex& destinationParent, int destinationFirst) {
			callback(AbstractItemModel::ItemIndex(sourceParent.row_, 0, sourceParent.parent_), sourceFirst, sourceLast,
			         AbstractItemModel::ItemIndex(destinationParent.row_, 0, destinationParent.parent_),
			         destinationFirst);
		});
	}

	bool moveRows(const AbstractItem* sourceParent, int sourceRow, int count, const AbstractItem* destinationParent,
	              int destinationChild) override
	{
		if (sourceParent != nullptr)
		{
			return false;
		}
		if (destinationParent != nullptr)
		{
			return false;
		}

		return moveRows(sourceRow, count, destinationChild);
	}

	Connection connectPostRowsMoved(AbstractItemModel::MoveCallback callback) override
	{
		return connectPostRowsMoved((MoveCallback)[=](const ItemIndex& sourceParent, int sourceFirst, int sourceLast,
		                                              const ItemIndex& destinationParent, int destinationFirst) {
			callback(AbstractItemModel::ItemIndex(sourceParent.row_, 0, sourceParent.parent_), sourceFirst, sourceLast,
			         AbstractItemModel::ItemIndex(destinationParent.row_, 0, destinationParent.parent_),
			         destinationFirst);
		});
	}
};

/** Provide models with items arranged in a grid; a table.
Items in each (row, column) can be unrelated. */
class AbstractTableModel : public AbstractItemModel
{
public:
	/** Structure for indexing a table model.
	It is a combination of a row number and a column number. */
	struct ItemIndex
	{
		ItemIndex(int row = -1, int column = -1) : row_(row), column_(column)
		{
		}

		/** Checks whether the index has valid data.
		An index is valid if neither the row, nor the column are negative. */
		bool isValid() const
		{
			return row_ >= 0 && column_ >= 0;
		}

		/** Checks equality of two indices.
		If two indices are both invalid, they are seen as equal, regardless of their actual values. */
		bool operator==(const ItemIndex& other) const
		{
			if (!isValid() && !other.isValid())
			{
				return true;
			}

			return row_ == other.row_ && column_ == other.column_;
		}

		/** Checks inequality of two indices.
		If two indices are both invalid, they are seen as equal, regardless of their actual values. */
		bool operator!=(const ItemIndex& other) const
		{
			return !this->operator==(other);
		}

		int row_;
		int column_;
	};

	/** Data changed at *index*, with the given role and new value.
	@note newValue is always the new value, for both pre- and post- callbacks. */
	typedef void DataSignature(const ItemIndex& index, ItemRole::Id role, const Variant& newValue);
	typedef std::function<DataSignature> DataCallback;

	/** Insert/remove from *startPos* to *startPos + count*.
	@param startPos first row or column of insertion/removal.
	@param count number of rows or columns after startPos. */
	typedef void RangeSignature(int startPos, int count);
	typedef std::function<RangeSignature> RangeCallback;

	/** Move items under *sourceParentIndex* from *sourceFirst* to *sourceLast* into *destinationRow*.
	@param sourceFirst first row or column of removal under the parent.
	@param sourceLast last row or column of removal under the parent.
	@param destinationRow first row or column for insertion under the parent. */
	typedef void MoveSignature(int sourceFirst, int sourceLast, int destinationFirst);
	typedef std::function<MoveSignature> MoveCallback;

	virtual ~AbstractTableModel()
	{
	}

	/** Get the AbstractItem pointer at the given ItemIndex.
	@note This is a specialization of the inherited method, removing irrelevant parameters.
	@param index The index of the item.
	@return The AbstractItem pointer if found, otherwise nullptr. */
	virtual AbstractItem* item(const ItemIndex& index) const = 0;

	/** Finds the index of an AbstractItem pointer.
	@param item The AbstractItem pointer to find.
	@return The index to return. */
	virtual ItemIndex index(const AbstractItem* item) const = 0;

	/** Returns the number of rows.
	@return The number of rows. */
	virtual int rowCount() const = 0;

	/** Returns the number of columns.
	@return The number of columns. */
	virtual int columnCount() const = 0;

	/** Inserts new rows with default values at the specified position.
	@note If successful, the pre and post signals need to be fired.
	@param row Row position to insert at.
	@param count Number of rows to insert.
	@return True if a row was inserted. */
	virtual bool insertRows(int row, int count)
	{
		return false;
	}

	/** Inserts new columns at the specified position.
	@note If successful, the pre and post signals need to be fired.
	@param column Column position to insert at.
	@param count Number of columns to insert.
	@return True if a column was inserted. */
	virtual bool insertColumns(int column, int count)
	{
		return false;
	}

	/** Removes rows at the specified position.
	@note If successful, the pre and post signals need to be fired.
	@param row Row position to remove.
	@param count Number of rows to remove.
	@return True if a row was removed. */
	virtual bool removeRows(int row, int count)
	{
		return false;
	}

	/** Removes columns at the specified position.
	@note If successful, the pre and post signals need to be fired.
	@param column Column position to remove.
	@param count Number of columns to remove.
	@return True if a column was removed. */
	virtual bool removeColumns(int column, int count)
	{
		return false;
	}

	/** Moves a number of rows from one position to another.
	@note If successful, the pre and post signals need to be fired.
	@param sourceRow The starting row number to move.
	@param count The amount of rows to move.
	@param destinationChild The target row number for the first row to move.
	@return True if the rows were moved. */
	virtual bool moveRows(int sourceRow, int count, int destinationRow)
	{
		return false;
	}

	/** Adds a callback to call before an item's data changed.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPreItemDataChanged(DataCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call after an item's data changed.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPostItemDataChanged(DataCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call before rows are inserted.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPreRowsInserted(RangeCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call after rows were inserted.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPostRowsInserted(RangeCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call before rows are removed.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPreRowsRemoved(RangeCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call after rows were removed.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPostRowsRemoved(RangeCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call before columns are inserted.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPreColumnsInserted(RangeCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call after columns were inserted.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPostColumnsInserted(RangeCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call before columns are removed.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPreColumnsRemoved(RangeCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call after columns were removed.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPostColumnsRemoved(RangeCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call before rows are moved.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPreRowsMoved(MoveCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call after rows are moved.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPostRowsMoved(MoveCallback callback)
	{
		return Connection();
	}

private:
	// Override functions using base class ItemIndex and hide them
	// Users can use public overloads with current class' ItemIndex type

	AbstractItem* item(const AbstractItemModel::ItemIndex& index) const override
	{
		if (!index.isValid())
		{
			return nullptr;
		}

		if (index.parent_ != nullptr)
		{
			return nullptr;
		}

		return item(ItemIndex(index.row_, index.column_));
	}

	void index(const AbstractItem* item, AbstractItemModel::ItemIndex& o_Index) const override
	{
		ItemIndex index = this->index(item);

		o_Index.row_ = index.row_;
		o_Index.column_ = index.column_;
		o_Index.parent_ = nullptr;
	}

	int rowCount(const AbstractItem* item) const override
	{
		if (item != nullptr)
		{
			return 0;
		}

		return rowCount();
	}

	int columnCount(const AbstractItem* item) const override
	{
		return columnCount();
	}

	bool insertRows(int row, int count, const AbstractItem* parent) override
	{
		if (parent != nullptr)
		{
			return false;
		}

		return insertRows(row, count);
	}

	bool insertColumns(int column, int count, const AbstractItem* parent) override
	{
		if (parent != nullptr)
		{
			return false;
		}

		return insertColumns(column, count);
	}

	bool removeRows(int row, int count, const AbstractItem* parent) override
	{
		if (parent != nullptr)
		{
			return false;
		}

		return removeRows(row, count);
	}

	bool removeColumns(int column, int count, const AbstractItem* parent) override
	{
		if (parent != nullptr)
		{
			return false;
		}

		return removeColumns(column, count);
	}

	bool moveRows(const AbstractItem* sourceParent, int sourceRow, int count, const AbstractItem* destinationParent,
	              int destinationChild) override
	{
		if (sourceParent != nullptr)
		{
			return false;
		}
		if (destinationParent != nullptr)
		{
			return false;
		}

		return moveRows(sourceRow, count, destinationChild);
	}

	Connection connectPreItemDataChanged(AbstractItemModel::DataCallback callback) override
	{
		return connectPreItemDataChanged(
		(DataCallback)[=](const ItemIndex& index, ItemRole::Id role, const Variant& value) {
			callback(AbstractItemModel::ItemIndex(index.row_, index.column_, nullptr), role, value);
		});
	}

	Connection connectPostItemDataChanged(AbstractItemModel::DataCallback callback) override
	{
		return connectPostItemDataChanged(
		(DataCallback)[=](const ItemIndex& index, ItemRole::Id role, const Variant& value) {
			callback(AbstractItemModel::ItemIndex(index.row_, index.column_, nullptr), role, value);
		});
	}

	Connection connectPreRowsInserted(AbstractItemModel::RangeCallback callback) override
	{
		return connectPreRowsInserted(
		(RangeCallback)[=](int pos, int count) { callback(AbstractItemModel::ItemIndex(), pos, count); });
	}

	Connection connectPostRowsInserted(AbstractItemModel::RangeCallback callback) override
	{
		return connectPostRowsInserted(
		(RangeCallback)[=](int pos, int count) { callback(AbstractItemModel::ItemIndex(), pos, count); });
	}

	Connection connectPreRowsRemoved(AbstractItemModel::RangeCallback callback) override
	{
		return connectPreRowsRemoved(
		(RangeCallback)[=](int pos, int count) { callback(AbstractItemModel::ItemIndex(), pos, count); });
	}

	Connection connectPostRowsRemoved(AbstractItemModel::RangeCallback callback) override
	{
		return connectPostRowsRemoved(
		(RangeCallback)[=](int pos, int count) { callback(AbstractItemModel::ItemIndex(), pos, count); });
	}

	Connection connectPreColumnsInserted(AbstractItemModel::RangeCallback callback) override
	{
		return connectPreColumnsInserted(
		(RangeCallback)[=](int pos, int count) { callback(AbstractItemModel::ItemIndex(), pos, count); });
	}

	Connection connectPostColumnsInserted(AbstractItemModel::RangeCallback callback) override
	{
		return connectPostColumnsInserted(
		(RangeCallback)[=](int pos, int count) { callback(AbstractItemModel::ItemIndex(), pos, count); });
	}

	Connection connectPreColumnsRemoved(AbstractItemModel::RangeCallback callback) override
	{
		return connectPreColumnsRemoved(
		(RangeCallback)[=](int pos, int count) { callback(AbstractItemModel::ItemIndex(), pos, count); });
	}

	Connection connectPostColumnsRemoved(AbstractItemModel::RangeCallback callback) override
	{
		return connectPostColumnsRemoved(
		(RangeCallback)[=](int pos, int count) { callback(AbstractItemModel::ItemIndex(), pos, count); });
	}

	Connection connectPreRowsMoved(AbstractItemModel::MoveCallback callback) override
	{
		return connectPreRowsMoved((MoveCallback)[=](int sourceFirst, int sourceLast, int destinationFirst) {
			callback(AbstractItemModel::ItemIndex(), sourceFirst, sourceLast, AbstractItemModel::ItemIndex(),
			         destinationFirst);
		});
	}

	Connection connectPostRowsMoved(AbstractItemModel::MoveCallback callback) override
	{
		return connectPostRowsMoved((MoveCallback)[=](int sourceFirst, int sourceLast, int destinationFirst) {
			callback(AbstractItemModel::ItemIndex(), sourceFirst, sourceLast, AbstractItemModel::ItemIndex(),
			         destinationFirst);
		});
	}
};
} // end namespace wgt
#endif // ABSTRACT_ITEM_MODEL_HPP
