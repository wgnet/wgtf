#ifndef ABSTRACT_ITEM_HPP
#define ABSTRACT_ITEM_HPP

#include "core_common/signal.hpp"
#include "core_variant/variant.hpp"
#include "i_item_role.hpp"

namespace wgt
{
/** Stores data for an entry in a data model or on the data model itself. */
class AbstractItem
{
public:
	/** Change data in role *roleId* to *value* at position *row* and *column*. */
	typedef void DataSignature(int row, int column, ItemRole::Id roleId, const Variant& value);
	typedef std::function<DataSignature> DataCallback;

	virtual ~AbstractItem()
	{
	}

	/** Returns the value for a role at a row and column position.
	Roles are a way of providing properties without changing this interface.
	This method handles the reading of role values.
	@param row The row at which to get the data.
	@param column The column at which to get the data.
	@param roleId The role identifier the value relates to.
	@return The value for the specified role. */
	virtual Variant getData(int row, int column, ItemRole::Id roleId) const
	{
		return Variant();
	}

	/** Changes the value for a role at a row and column position.
	Roles are a way of providing properties without changing this interface.
	This method handles the changing of role values.
	@note If successful, the pre and post signals need to be fired.
	The following connect methods need to be implemented for this to work:
	connectPreDataChanged, connectPostDataChanged, connectPreItemDataChanged, connectPostItemDataChanged
	@param row The row at which to set the data.
	@param column The column at which to set the data.
	@param roleId The role identifier the value relates to.
	@param data The value for the specified role.
	@return True if successful. */
	virtual bool setData(int row, int column, ItemRole::Id roleId, const Variant& data)
	{
		return false;
	}

	/** Adds a callback to call before the item's data changed.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPreDataChanged(DataCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call after the item's data changed.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPostDataChanged(DataCallback callback)
	{
		return Connection();
	}

	/** @return true if this item uses a controller to call setData() via the Command System.
	@return False if the item sets data directly. */
	virtual bool hasController() const
	{
		return false;
	}
};

/** Specialization of AbstractItem to simplify use in a list. */
class AbstractListItem : public AbstractItem
{
public:
	/** Change data in role *roleId* to *value* at the *column* position. */
	typedef void DataSignature(int column, ItemRole::Id role, const Variant& value);
	typedef std::function<DataSignature> DataCallback;

	virtual ~AbstractListItem()
	{
	}

	/** Returns the value for a role at a column position.
	@note This is a specialization of the inherited method, removing irrelevant parameters.
	@param column The column at which to get the data.
	@param roleId The role identifier the value relates to.
	@return The value for the specified role. */
	virtual Variant getData(int column, ItemRole::Id roleId) const
	{
		return Variant();
	}

	/** Changes the value for a role at a column position.
	@note This is a specialization of the inherited method, removing irrelevant parameters.
	@param column The column at which to set the data.
	@param roleId The role identifier the value relates to.
	@param data The value for the specified role.
	@return True if successful. */
	virtual bool setData(int column, ItemRole::Id roleId, const Variant& data)
	{
		return false;
	}

	/** Adds a callback to call before the item's data changed.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPreDataChanged(DataCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call after the item's data changed.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPostDataChanged(DataCallback callback)
	{
		return Connection();
	}

private:
	Variant getData(int row, int column, ItemRole::Id roleId) const override
	{
		return getData(column, roleId);
	}

	bool setData(int row, int column, ItemRole::Id roleId, const Variant& data) override
	{
		return setData(column, roleId, data);
	}

	Connection connectPreDataChanged(AbstractItem::DataCallback callback) override
	{
		return connectPreDataChanged(
		(DataCallback)[=](int column, ItemRole::Id role, const Variant& value) { callback(0, column, role, value); });
	}

	Connection connectPostDataChanged(AbstractItem::DataCallback callback) override
	{
		return connectPostDataChanged(
		(DataCallback)[=](int column, ItemRole::Id role, const Variant& value) { callback(0, column, role, value); });
	}
};

typedef AbstractListItem AbstractTreeItem;

/** Specialization of AbstractItem to simplify use in a table. */
class AbstractTableItem : public AbstractItem
{
public:
	/** Change data in role *roleId* to *value*. */
	typedef void DataSignature(ItemRole::Id role, const Variant& value);
	typedef std::function<DataSignature> DataCallback;

	virtual ~AbstractTableItem()
	{
	}

	/** Returns the value for a role.
	@note This is a specialization of the inherited method, removing irrelevant parameters.
	@param roleId The role identifier the value relates to.
	@return The value for the specified role. */
	virtual Variant getData(ItemRole::Id roleId) const
	{
		return Variant();
	}

	/** Changes the value for a role.
	@note This is a specialization of the inherited method, removing irrelevant parameters.
	@param roleId The role identifier the value relates to.
	@param data The value for the specified role.
	@return True if successful. */
	virtual bool setData(ItemRole::Id roleId, const Variant& data)
	{
		return false;
	}

	/** Adds a callback to call before the item's data changed.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPreDataChanged(DataCallback callback)
	{
		return Connection();
	}

	/** Adds a callback to call after the item's data changed.
	@note This is a specialization of the inherited method, using a simplified callback.
	@param callback The callback to call.
	@return A Connection between the Signal and the callback. */
	virtual Connection connectPostDataChanged(DataCallback callback)
	{
		return Connection();
	}

private:
	Variant getData(int row, int column, ItemRole::Id roleId) const override
	{
		return getData(roleId);
	}

	bool setData(int row, int column, ItemRole::Id roleId, const Variant& data) override
	{
		return setData(roleId, data);
	}

	Connection connectPreDataChanged(AbstractItem::DataCallback callback) override
	{
		return connectPreDataChanged(
		(DataCallback)[=](ItemRole::Id role, const Variant& value) { callback(0, 0, role, value); });
	}

	Connection connectPostDataChanged(AbstractItem::DataCallback callback) override
	{
		return connectPostDataChanged(
		(DataCallback)[=](ItemRole::Id role, const Variant& value) { callback(0, 0, role, value); });
	}
};
} // end namespace wgt
#endif // ABSTRACT_ITEM_HPP
