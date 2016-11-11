#include "test_tree_item.hpp"
#include "core_data_model/i_item_role.hpp"

namespace wgt
{
ITEMROLE(display)
ITEMROLE(value)

struct TestTreeItem::Implementation
{
	Implementation(TestTreeItem& main, const std::string& name, const AbstractTreeItem* parent);

	AbstractTreeItem& main_;
	std::string name_;
	const AbstractTreeItem* parent_;

	Signal<DataSignature> preDataChanged_;
	Signal<DataSignature> postDataChanged_;
};

TestTreeItem::Implementation::Implementation(TestTreeItem& main, const std::string& name,
                                             const AbstractTreeItem* parent)
    : main_(main), name_(name), parent_(parent)
{
}

TestTreeItem::TestTreeItem(const std::string& name, const AbstractTreeItem* parent)
    : impl_(new Implementation(*this, name, parent))
{
}

TestTreeItem::~TestTreeItem()
{
}

void TestTreeItem::setParent(AbstractTreeItem* parent)
{
	impl_->parent_ = parent;
}

const AbstractTreeItem* TestTreeItem::getParent() const
{
	return impl_->parent_;
}

Variant TestTreeItem::getData(int column, ItemRole::Id roleId) const
{
	if (column == 0)
	{
		if (roleId == ItemRole::displayId || roleId == ItemRole::valueId)
		{
			return impl_->name_;
		}
	}
	else
	{
		if (roleId == ItemRole::displayId || roleId == ItemRole::valueId)
		{
			return "Value";
		}
	}

	return Variant();
}

bool TestTreeItem::setData(int column, ItemRole::Id roleId, const Variant& data) /* override */
{
	if (column == 0)
	{
		if (roleId == ItemRole::displayId || roleId == ItemRole::valueId)
		{
			std::string name;
			if (data.tryCast(name))
			{
				impl_->preDataChanged_(column, roleId, data);
				impl_->name_ = name;
				impl_->postDataChanged_(column, roleId, data);
				return true;
			}
		}
	}

	return false;
}

Connection TestTreeItem::connectPreDataChanged(DataCallback callback) /* override */
{
	return impl_->preDataChanged_.connect(callback);
}

Connection TestTreeItem::connectPostDataChanged(DataCallback callback) /* override */
{
	return impl_->postDataChanged_.connect(callback);
}

} // end namespace wgt
