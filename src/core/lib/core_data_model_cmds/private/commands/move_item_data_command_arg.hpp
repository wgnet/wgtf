#pragma once

#include "core_reflection/reflected_object.hpp"

namespace wgt
{
class AbstractItem;
class AbstractItemModel;

/**
 *	Command data for inserting/removing rows/columns on an AbstractItemModel.
 */
class MoveItemDataCommandArgument
{
	DECLARE_REFLECTED

public:
	enum Direction
	{
		ROW,
		COLUMN
	};

	/**
	 *	Construct with the builder pattern, using set functions below.
	 */
	MoveItemDataCommandArgument();

	void setModel(AbstractItemModel& model);
	void setStartPos(int startPos);
	void setDirection(Direction dir);
	void setEndPos(int endPos);
	void setCount(int count);
	void setStartParent(const AbstractItem* startParent);
	void setEndParent(const AbstractItem* endParent);

	Direction direction_;
	AbstractItemModel* pModel_;
	const AbstractItem* startParent_;
	int startPos_;
	const AbstractItem* endParent_;
	int endPos_;
	int count_;
};

} // end namespace wgt
