#pragma once

#include "core_data_model/abstract_item_model.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_variant/variant.hpp"

#include <vector>

namespace wgt
{
/**
 *	Command data for inserting rows/columns on an AbstractItemModel.
 */
class InsertRowsCommandArgument
{
	DECLARE_REFLECTED

public:
	enum Type
	{
		ROW,
		COLUMN
	};

	/**
	 *	Construct with the builder pattern, using set functions below.
	 */
	InsertRowsCommandArgument();

	void setModel(AbstractItemModel& model);
	void setStartPos(int startPos, Type type);
	void setCount(int count);
	void setParent(const AbstractItem* pParent);

	AbstractItemModel* pModel_;
	int startPos_;
	Type type_;
	int count_;
	const AbstractItem* pParent_;
};

} // end namespace wgt
