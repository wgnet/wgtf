#include "insert_item_command_arg.hpp"
#include "core_data_model/collection_model.hpp"
#include "core_data_model/abstract_item_model.hpp"

namespace wgt
{
InsertItemCommandArgument::InsertItemCommandArgument()
    : pModel_(nullptr)
{
}

void InsertItemCommandArgument::setModel(CollectionModel& model)
{
	pModel_ = &static_cast<AbstractListModel&>(model);
}

void InsertItemCommandArgument::setKey(const Variant& key)
{
	key_ = key;
}

void InsertItemCommandArgument::setValue(const Variant& value)
{
	value_ = value;
}

} // end namespace wgt
