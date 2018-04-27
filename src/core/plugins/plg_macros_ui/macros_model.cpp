#include "macros_model.hpp"

#include "core_common/assert.hpp"
#include "core_data_model/i_item_role.hpp"
#include "core_data_model/common_data_roles.hpp"
#include "core_command_system/command.hpp"
#include "core_command_system/i_command_manager.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "core_variant/collection.hpp"
#include "macro_display.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_data_model/reflection_proto/property_tree_model.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"

namespace wgt
{
ITEMROLE(commandName)
ITEMROLE(commandParameters)

namespace Macros_Detail
{
class MacroItem : public CollectionItem, Depends<IDefinitionManager>
{
public:
	MacroItem(CollectionModel& model, size_t index) : CollectionItem(model, index)
	{
	}

	virtual Variant getData(int row, int column, ItemRole::Id roleId) const override
	{
		auto& collection = model_.getSource();
		if (row < 0 || row >= static_cast<int>(collection.size()))
		{
			return Variant();
		}

		if (roleId == ItemRole::valueId)
		{
			if (display_ == nullptr)
			{
				display_ = ManagedObject<MacroDisplay>::make();
                displayHandle_ = display_.getHandleT();
				auto& collection = model_.getSource();
				auto it = collection[row];
				CompoundCommand* macro = nullptr;
				macro = it.cast<CompoundCommand*>();
				TF_ASSERT(macro != nullptr);
				display_->init(macro);
			}
			return displayHandle_;
		}

		return Variant();
	}

	virtual bool setData(int row, int column, ItemRole::Id roleId, const Variant& data) override
	{
		auto& collection = model_.getSource();
		if (row < 0 || row >= static_cast<int>(collection.size()))
		{
			return false;
		}
		if (roleId == ItemRole::valueId)
		{
			ObjectHandle display = data.cast<ObjectHandle>();
            displayHandle_ = safeCast<MacroDisplay>(display);
            collection[row] = displayHandle_->getMacro();
			return true;
		}
		return false;
	}

private:
	mutable ManagedObject<MacroDisplay> display_;
    mutable ObjectHandleT<MacroDisplay> displayHandle_;
};

class MacroStepItem : public CollectionItem, Depends<ICommandManager>
{
public:
	MacroStepItem(CollectionModel& model, size_t index) : CollectionItem(model, index)
	{
		auto& collection = model_.getSource();
		TF_ASSERT(!(index < 0 || index >= collection.size()));
		auto it = collection[index];
		auto pair = it.cast<std::pair<std::string, ObjectHandle>>();
		auto arg = pair.second;
		argModel_.reset(new proto::PropertyTreeModel(arg));
	}

	virtual Variant getData(int row, int column, ItemRole::Id roleId) const override
	{
		auto& collection = model_.getSource();
		if (row < 0 || row >= static_cast<int>(collection.size()))
		{
			return Variant();
		}
		auto commandManager = get<ICommandManager>();
		TF_ASSERT(commandManager != nullptr);
		auto it = collection[row];
		auto pair = it.cast<std::pair<std::string, ObjectHandle>>();
		auto commandId = pair.first;
		auto arg = pair.second;
		auto command = commandManager->findCommand(commandId.c_str());
		TF_ASSERT(command != nullptr);
		if (roleId == ItemRole::commandNameId)
		{
			return command->getName();
		}
		else if (roleId == ItemRole::commandParametersId)
		{
			return argModel_.get();
		}
		return Variant();
	}

	virtual bool setData(int row, int column, ItemRole::Id roleId, const Variant& data) override
	{
		return false;
	}

private:
	std::unique_ptr<AbstractTreeModel> argModel_;
};

} // end namespace Macros_Detail

//////////////////////////////////////////////////////////////////////////
MacrosModel::MacrosModel()
{
}

MacrosModel::~MacrosModel()
{
}

AbstractItem* MacrosModel::item(int index) const
{
	// Do not create items past the end of the collection
	if (static_cast<size_t>(index) >= collection_.size())
	{
		return nullptr;
	}

	if (items_.size() <= (size_t)index)
	{
		items_.resize(index + 1);
	}

	auto item = items_[index].get();
	if (item != nullptr)
	{
		return item;
	}

	item = new Macros_Detail::MacroItem(*const_cast<MacrosModel*>(this), index);
	items_[index] = std::unique_ptr<AbstractItem>(item);
	return item;
}

//////////////////////////////////////////////////////////////////////////
namespace MacroStepsModelDetails
{
static const std::string s_RolesArr[] = { ItemRole::commandParametersName, ItemRole::commandNameName };
static const std::vector<std::string> s_RolesVec(&s_RolesArr[0],
                                                 &s_RolesArr[0] + std::extent<decltype(s_RolesArr)>::value);
}

//------------------------------------------------------------------------------
MacroStepsModel::MacroStepsModel()
{
}

MacroStepsModel::~MacroStepsModel()
{
}

AbstractItem* MacroStepsModel::item(int index) const
{
	// Do not create items past the end of the collection
	if (static_cast<size_t>(index) >= collection_.size())
	{
		return nullptr;
	}

	if (items_.size() <= (size_t)index)
	{
		items_.resize(index + 1);
	}

	auto item = items_[index].get();
	if (item != nullptr)
	{
		return item;
	}

	item = new Macros_Detail::MacroStepItem(*const_cast<MacroStepsModel*>(this), index);
	items_[index] = std::unique_ptr<AbstractItem>(item);
	return item;
}

//------------------------------------------------------------------------------
void MacroStepsModel::iterateRoles(const std::function<void(const char*)>& iterFunc) const
{
	CollectionModel::iterateRoles(iterFunc);
	for (auto&& role : MacroStepsModelDetails::s_RolesVec)
	{
		iterFunc(role.c_str());
	}
}

//------------------------------------------------------------------------------
std::vector<std::string> MacroStepsModel::roles() const
{
	std::vector<std::string> roles;
	roles = CollectionModel::roles();
	roles.insert(roles.end(), MacroStepsModelDetails::s_RolesVec.begin(), MacroStepsModelDetails::s_RolesVec.end());
	return roles;
}
} // end namespace wgt
