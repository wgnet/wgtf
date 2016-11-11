#ifndef GENERIC_TREE_ITEM_HPP
#define GENERIC_TREE_ITEM_HPP

#include "i_item.hpp"

namespace wgt
{
class GenericTreeModel;

class GenericTreeItem : public IItem
{
public:
	GenericTreeItem();
	virtual ~GenericTreeItem();

	// TODO remove this from the generic model and move into a filter
	inline bool hidden() const
	{
		return hidden_;
	}
	inline void hidden(bool value)
	{
		hidden_ = value;
	}

	GenericTreeModel* getModel() const
	{
		return model_ != nullptr ? model_ : getParent()->getModel();
	}

	virtual GenericTreeItem* getParent() const = 0;
	virtual GenericTreeItem* getChild(size_t index) const = 0;

	virtual bool empty() const;
	virtual size_t size() const = 0;

private:
	GenericTreeModel* model_;
	bool hidden_;

	friend class GenericTreeModel;
};
} // end namespace wgt
#endif // GENERIC_TREE_ITEM_HPP
