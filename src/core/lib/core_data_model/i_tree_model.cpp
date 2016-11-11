#include "i_tree_model.hpp"
#include "core_variant/variant.hpp"

namespace wgt
{
IItem* ITreeModel::item(ItemIndex index) const
{
	return this->item(index.first, index.second);
}

bool ITreeModel::empty(const IItem* item) const
{
	return (this->size(item) == 0);
}

Variant ITreeModel::getData(int column, ItemRole::Id roleId) const
{
	return Variant();
}

bool ITreeModel::setData(int column, ItemRole::Id roleId, const Variant& data)
{
	return false;
}
} // end namespace wgt
