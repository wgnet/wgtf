#include "insert_default_command_arg.hpp"
#include "core_data_model/collection_model.hpp"
#include "core_data_model/abstract_item_model.hpp"

namespace wgt
{
InsertDefaultCommandArgument::InsertDefaultCommandArgument() : pModel_(nullptr)
{
}

void InsertDefaultCommandArgument::setModel(CollectionModel& model)
{
	pModel_ = &static_cast<AbstractListModel&>(model);
}

void InsertDefaultCommandArgument::setKey(const Variant& key)
{
	key_ = key;
}

} // end namespace wgt
