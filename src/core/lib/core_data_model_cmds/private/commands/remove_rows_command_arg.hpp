#pragma once

#include "core_data_model/abstract_item_model.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_variant/variant.hpp"

#include <vector>

namespace wgt
{
/**
 *	Command data for inserting/removing rows/columns on an AbstractItemModel.
 */
class RemoveRowsCommandArgument
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
	RemoveRowsCommandArgument();

	void setModel(AbstractItemModel& model);
	void setStartPos(int startPos, Type type);
	void setCount(int count);
	void setParent(const AbstractItem* pParent);
	void saveRows();

	AbstractItemModel* pModel_;
	int startPos_;
	Type type_;
	int count_;
	const AbstractItem* pParent_;
	struct ExtractedRows
	{
		AbstractItemModel::ItemIndex index_;
		ItemRole::Id roleId_;
		Variant data_;
	};
	typedef std::vector<ExtractedRows> ExtractedRowsStorage;
	ExtractedRowsStorage itemData_;
};

} // end namespace wgt
