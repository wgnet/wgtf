#include "insert_rows_command_arg.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_variant/collection.hpp"

namespace wgt
{
InsertRowsCommandArgument::InsertRowsCommandArgument()
    : pModel_(nullptr)
    , startPos_(-1)
    , type_(Type::ROW)
    , count_(-1)
    , pParent_(nullptr)
{
}

void InsertRowsCommandArgument::setModel(AbstractItemModel& model)
{
	pModel_ = &model;
}

void InsertRowsCommandArgument::setStartPos(int startPos, Type type)
{
	startPos_ = startPos;
	type_ = type;
}

void InsertRowsCommandArgument::setCount(int count)
{
	count_ = count;
}

void InsertRowsCommandArgument::setParent(const AbstractItem* pParent)
{
	pParent_ = pParent;
}

} // end namespace wgt
