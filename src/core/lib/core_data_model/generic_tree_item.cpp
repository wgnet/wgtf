#include "generic_tree_item.hpp"

namespace wgt
{
GenericTreeItem::GenericTreeItem() : model_(nullptr), hidden_(false)
{
}

GenericTreeItem::~GenericTreeItem()
{
}

bool GenericTreeItem::empty() const
{
	return (this->size() == 0);
}
} // end namespace wgt
