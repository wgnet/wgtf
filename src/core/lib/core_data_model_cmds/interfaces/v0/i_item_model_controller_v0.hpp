#pragma once

#include "core_data_model/abstract_item_model.hpp"
#include "core_dependency_system/i_interface.hpp"

namespace wgt
{
class CollectionModel;
class Variant;

/**
 *	Interface for modifying core_data_model via the core_command_system.
 */
DECLARE_INTERFACE_BEGIN( IItemModelController, 0, 0 )
	/**
	 *	Set a value on an item via a data model.
	 *	This will block to wait for the return value.
	 *	@param model the model on which to set the data.
	 *		The model should fire signals appropriately when the set is executed.
	 *	@param index to the item in the model.
	 *	@param role for the data in the model.
	 *	@param data new value of the item.
	 *	@return true if the set command was run successfully.
	 *		Command will block to wait for result.
	 */
	virtual bool setValue( AbstractItemModel & model,
		const AbstractItemModel::ItemIndex & index,
		size_t roleId,
		const Variant & data ) = 0;

	/**
	 *	Set data on the data model itself.
	 *	This will block to wait for the return value.
	 *	@param model the model on which to set the data.
	 *		The model should fire signals appropriately when the set is executed.
	 *	@param row to the data in the model.
	 *	@param column to the data in the model.
	 *	@param role for the data in the model.
	 *	@param data new value.
	 *	@return true if the set command was run successfully.
	 *		Command will block to wait for result.
	 */
	virtual bool setModelData( AbstractItemModel & model,
		int row,
		int column,
		size_t roleId,
		const Variant & data ) = 0;

	/**
	 *	Insert rows into a data model.
	 *	This will block to wait for the return value.
	 *	@param model the model on which to insert the rows.
	 *		The model should fire signals appropriately when the set is executed.
	 *	@param row *after* which to insert the new rows.
	 *	@param count number of items to be inserted *after* row.
	 *	@param pParent parent item of the row or nullptr.
	 *	@return true if the set command was run successfully.
	 *		Command will block to wait for result.
	 */
	virtual bool insertRows( AbstractItemModel & model,
		int row,
		int count,
		const AbstractItem * pParent ) = 0;

	/**
	 *	Insert columns into a data model.
	 *	This will block to wait for the return value.
	 *	@param model the model on which to insert the columns.
	 *		The model should fire signals appropriately when the set is executed.
	 *	@param column *after* which to insert the new columns.
	 *	@param count number of items to be inserted *after* column.
	 *	@param pParent parent item of the column or nullptr.
	 *	@return true if the set command was run successfully.
	 *		Command will block to wait for result.
	 */
	virtual bool insertColumns( AbstractItemModel & model,
		int column,
		int count,
		const AbstractItem * pParent ) = 0;

	/**
	 *	Remove rows in a data model.
	 *	This will block to wait for the return value.
	 *	@param model the model on which to remove the rows.
	 *		The model should fire signals appropriately when the set is executed.
	 *	@param row from which to remove (inclusive).
	 *	@param count number of items to be removed after row.
	 *	@param pParent parent item of the row or nullptr.
	 *	@return true if the set command was run successfully.
	 *		Command will block to wait for result.
	 */
	virtual bool removeRows( AbstractItemModel & model,
		int row,
		int count,
		const AbstractItem * pParent ) = 0;

	/**
	 *	Remove columns in a data model.
	 *	This will block to wait for the return value.
	 *	@param model the model on which to remove the columns.
	 *		The model should fire signals appropriately when the set is executed.
	 *	@param column from which to remove (inclusive).
	 *	@param count number of items to be removed after column.
	 *	@param pParent parent item of the column or nullptr.
	 *	@return true if the set command was run successfully.
	 *		Command will block to wait for result.
	 */
	virtual bool removeColumns( AbstractItemModel & model,
		int column,
		int count,
		const AbstractItem * pParent ) = 0;

	/**
	*	Move rows within a data model.
	*	This will block to wait for the return value.
	*	@param model he model on which to move the rows.
	*		The model should fire signals appropriately when the set is executed.
	*	@param sourceParent
	*	@param sourceRow
	*	@param count
	*	@param destParent
	*	@param destRow
	*	@return true if the set command was run successfully.
	*		Command will block to wait for result.
	*/
	virtual bool moveRows( AbstractItemModel & model,
		const AbstractItem * sourceParent,
		int sourceRow,
		int count,
		const AbstractItem * destParent,
		int destRow ) = 0;

    /**
	 *	Insert a default-initialized item into a collection model.
	 *	This will block to wait for the return value.
	 *	@param model the model on which to insert the item.
	 *		The model should fire signals appropriately when the insert is executed.
	 *	@param key at which to perform the insertion.
	 *	@return true if the command was run successfully.
	 *		Command will block to wait for result.
	 */
    virtual bool insertItem(CollectionModel& model,
                            const Variant& key) = 0;

    /**
	 *	Insert an item into a collection model.
	 *	This will block to wait for the return value.
	 *	@param model the model on which to insert the item.
	 *		The model should fire signals appropriately when the insert is executed.
	 *	@param key at which to perform the insertion.
	 *	@param data to initialize the item.
	 *	@return true if the command was run successfully.
	 *		Command will block to wait for result.
	 */
    virtual bool insertItem(CollectionModel& model,
                            const Variant& key,
                            const Variant& data) = 0;

    /**
	 *	Remove an item from a collection model.
	 *	This will block to wait for the return value.
	 *	@param model the model on which to remove the item.
	 *		The model should fire signals appropriately when the remove is executed.
	 *	@param key at which to perform the removal.
	 *	@return true if the command was run successfully.
	 *		Command will block to wait for result.
	 */
    virtual bool removeItem(CollectionModel& model, const Variant& key) = 0;

DECLARE_INTERFACE_END()

} // end namespace wgt
