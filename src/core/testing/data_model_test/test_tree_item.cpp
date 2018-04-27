#include "test_tree_item.hpp"
#include "core_data_model/i_item_role.hpp"

namespace wgt
{
ITEMROLE(display)
ITEMROLE(value)
ITEMROLE(indexPath)
ITEMROLE(fullPath)
ITEMROLE(tooltip)

struct TestTreeItem::Implementation
{
	Implementation(TestTreeItem& main, const std::string& name, const TestTreeItem* parent);

	AbstractTreeItem& main_;
	std::string name_;
	const TestTreeItem* parent_;

	Signal<DataSignature> preDataChanged_;
	Signal<DataSignature> postDataChanged_;
};

TestTreeItem::Implementation::Implementation(TestTreeItem& main, const std::string& name,
                                             const TestTreeItem* parent)
    : main_(main), name_(name), parent_(parent)
{
}

TestTreeItem::TestTreeItem(const std::string& name, const TestTreeItem* parent)
    : impl_(new Implementation(*this, name, parent))
{
}

TestTreeItem::~TestTreeItem()
{
}

void TestTreeItem::setParent(TestTreeItem* parent)
{
	impl_->parent_ = parent;
}

const TestTreeItem* TestTreeItem::getParent() const
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
		else if (roleId == ItemRole::fullPathId || roleId == ItemRole::indexPathId || roleId == ItemRole::tooltipId)
		{
			std::string path = impl_->name_;
			auto parent = impl_->parent_;
			while (parent != nullptr)
			{
				path = parent->getData(0, ItemRole::displayId).cast<std::string>() + "/" + path;
				parent = parent->getParent();
			}
			return path;
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
