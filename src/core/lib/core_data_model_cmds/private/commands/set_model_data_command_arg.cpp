#include "set_model_data_command_arg.hpp"

#include "core_common/assert.hpp"
#include "core_data_model/abstract_item_model.hpp"

#include <limits>

namespace wgt
{
SetModelDataCommandArgument::SetModelDataCommandArgument()
    : pModel_(nullptr), row_(-1), column_(-1), roleId_(std::numeric_limits<size_t>::max())
{
}

void SetModelDataCommandArgument::setModel(AbstractItemModel& model)
{
	pModel_ = &model;
}

void SetModelDataCommandArgument::setIndex(int row, int column)
{
	row_ = row;
	column_ = column;
}

void SetModelDataCommandArgument::setValue(size_t roleId, const Variant& newValue)
{
	roleId_ = roleId;

	TF_ASSERT(pModel_ != nullptr);

	oldValue_ = pModel_->getData(row_, column_, roleId_);
	newValue_ = newValue;
}

} // end namespace wgt
