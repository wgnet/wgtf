#pragma once

#include "core_reflection/reflected_object.hpp"
#include "core_variant/variant.hpp"

namespace wgt
{
class AbstractItemModel;

/**
 *	Command data for setting data on an AbstractItemModel.
 */
class SetModelDataCommandArgument
{
	DECLARE_REFLECTED

public:
	/**
	 *	Construct with the builder pattern, using set functions below.
	 */
	SetModelDataCommandArgument();

	void setModel(AbstractItemModel& model);
	void setIndex(int row, int column);
	void setValue(size_t roleId, const Variant& newValue);

	AbstractItemModel* pModel_;
	int row_;
	int column_;
	size_t roleId_;
	Variant oldValue_;
	Variant newValue_;
};

} // end namespace wgt
