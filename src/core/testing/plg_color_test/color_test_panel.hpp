#pragma once

#include "color_tree_data.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_data_model/reflection_proto/reflected_tree_model.hpp"
#include "core_object/managed_object.hpp"

namespace wgt
{
class ColorTestPanel
{
	DECLARE_REFLECTED

public:
	ColorTestPanel()
	{
		data_ = ManagedObject<ColorTreeData>::make();
		tree_ = std::make_unique<proto::ReflectedTreeModel>(data_.getHandleT());
	}

	const AbstractTreeModel* getTreeModel() const
	{
		return tree_.get();
	}

private:
    ManagedObject<ColorTreeData> data_;
	std::unique_ptr<AbstractTreeModel> tree_;
};
}