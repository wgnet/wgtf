#pragma once

#include "core_data_model/abstract_item_model.hpp"
#include "core_dependency_system/i_interface.hpp"

namespace wgt
{

class Variant;

/**
 *	Interface for modifying core_data_model via the core_command_system.
 */
DECLARE_INTERFACE_BEGIN( IItemModelController, 0, 0 )
	/**
	 *	Set a value via a data model.
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
DECLARE_INTERFACE_END()

} // end namespace wgt
