#include "i_list_model.hpp"
#include "core_variant/variant.hpp"

namespace wgt
{
bool IListModel::empty() const
{
	return this->size() == 0;
}

Variant IListModel::getData(int column, ItemRole::Id roleId) const
{
	return Variant();
}

bool IListModel::setData(int column, ItemRole::Id roleId, const Variant& data)
{
	return false;
}
} // end namespace wgt
