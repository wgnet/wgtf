#pragma once

#include "core_reflection/reflected_object.hpp"
#include "core_data_model/abstract_item_model.hpp"

namespace wgt
{

/**
 *	Command data for setting data on an AbstractItemModel.
 */
class SetItemDataCommandArgument
{
	DECLARE_REFLECTED

public:
	/**
	 *	Construct with the builder pattern, using set functions below.
	 */
	SetItemDataCommandArgument();

	void setModel( AbstractItemModel & model );
	void setIndex( const AbstractItemModel::ItemIndex & index );
	void setValue( size_t roleId, const Variant & newValue );

	AbstractItemModel * pModel_;
	AbstractItemModel::ItemIndex index_;
	size_t roleId_;
	Variant oldValue_;
	Variant newValue_;
};


} // end namespace wgt
