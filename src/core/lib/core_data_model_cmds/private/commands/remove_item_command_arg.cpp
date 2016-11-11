#include "remove_item_command_arg.hpp"
#include "core_data_model/abstract_item_model.hpp"
#include "core_data_model/collection_model.hpp"

namespace wgt
{
RemoveItemCommandArgument::RemoveItemCommandArgument() : pModel_(nullptr)
{
}

void RemoveItemCommandArgument::setModel(CollectionModel& model)
{
	pModel_ = &static_cast<AbstractListModel&>(model);
}

void RemoveItemCommandArgument::setKey(const Variant& key)
{
	key_ = key;
}

void RemoveItemCommandArgument::setValue(const Variant& value)
{
	value_ = value;
}

} // end namespace wgt
