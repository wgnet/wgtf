//-----------------------------------------------------------------------------
//
//  mode_panel_view_model.cpp
//
//-----------------------------------------------------------------------------
//  Copyright (c) 2015-2015 Wargaming World, Ltd. All rights reserved.
//-----------------------------------------------------------------------------
#include "toolbox_panel.hpp"

#include <core_data_model/common_data_roles.hpp>
#include <core_data_model/reflection_proto/property_tree_model.hpp>
#include <core_reflection/metadata/meta_impl.hpp>
#include <core_reflection/metadata/meta_types.hpp>
#include <core_reflection/reflection_macros.hpp>
#include <core_reflection/utilities/reflection_function_utilities.hpp>
#include <core_string_utils/string_utils.hpp>
#include <core_ui_framework/i_ui_application.hpp>
#include <core_ui_framework/i_view.hpp>
#include <core_ui_framework/interfaces/i_view_creator.hpp>
#include <toolbox/i_tool.hpp>
#include <toolbox/i_toolbox.hpp>

namespace wgt
{
namespace ToolboxPanelDetails
{
static const std::string s_RolesArr[] = { ItemRole::valueName };
static const std::vector<std::string> s_RolesVec(&s_RolesArr[0],
                                                 &s_RolesArr[0] + std::extent<decltype(s_RolesArr)>::value);

class ToolObject : Depends<IDefinitionManager, IToolbox>
{
	DECLARE_REFLECTED
public:
	ToolObject(ObjectHandleT<ITool> tool) : tool_(tool)
	{
		auto definitionManager = get<IDefinitionManager>();
		definition_ = definitionManager->getObjectDefinition(reflectedRoot(tool_, *definitionManager));

		if (tool_ != nullptr)
		{
			connections +=
			tool_->connectCanActivateChanged([this](bool canActivate) { canActivateChanged_(Variant(canActivate)); });
		}

		TF_ASSERT(isValid());
	}

	std::string icon() const
	{
		auto definitionManager = get<IDefinitionManager>();
		auto icon = findFirstMetaData<MetaIconObj>(*definition_, *definitionManager);
		if (icon != nullptr)
		{
			return icon->getIcon();
		}
		else
		{
			if (auto toolbox = get<IToolbox>())
			{
				return toolbox->getDefaultIcon();
			}
		}

		return "";
	}

	std::string tooltip() const
	{
		auto definitionManager = get<IDefinitionManager>();
		auto displayName = findFirstMetaData<MetaDisplayNameObj>(*definition_, *definitionManager);
		if (displayName != nullptr)
		{
			return StringUtils::to_string(displayName->getDisplayName());
		}
		return definition_->getName();
	}

	bool canActivate() const
	{
		return tool_ != nullptr && tool_->canActivate();
	}

	bool active() const
	{
		if (auto toolbox = get<IToolbox>())
		{
			return tool_ == toolbox->getActiveTool();
		}
		return false;
	}

	void setActive(const bool&)
	{
		activeChanged_(Variant(this->active()));
	}

	bool isValid() const
	{
		return tool_ != nullptr && definition_ != nullptr;
	}

	void getActiveSignal(Signal<void(Variant&)>** result) const
	{
		*result = const_cast<Signal<void(Variant&)>*>(&activeChanged_);
	}

	void getCanActivateSignal(Signal<void(Variant&)>** result) const
	{
		*result = const_cast<Signal<void(Variant&)>*>(&canActivateChanged_);
	}

private:
	ObjectHandleT<ITool> tool_;
	IClassDefinition* definition_;
	ConnectionHolder connections;
	Signal<void(Variant&)> activeChanged_;
	Signal<void(Variant&)> canActivateChanged_;
};

class ToolsItem : public AbstractListItem
{
public:
	ToolsItem(ObjectHandleT<ITool> tool) : tool_(ManagedObject<ToolObject>::make(tool))
	{
	}

	virtual ~ToolsItem()
	{
	}

	virtual Variant getData(int column, ItemRole::Id roleId) const override
	{
		if (column != 0 || !tool_->isValid())
		{
			return Variant();
		}

		if (roleId == ItemRole::valueId)
		{
			return tool_.getHandleT();
		}

		return Variant();
	}

	void setActive(bool active)
	{
		tool_->setActive(active);
	}

private:
	ManagedObject<ToolObject> tool_;
};

class ToolsModel : public AbstractListModel, Depends<IToolbox>
{
public:
	ToolsModel()
	{
		refresh();
	}

	virtual ~ToolsModel() = default;

	virtual Variant getData(int row, int column, ItemRole::Id roleId) const override
	{
		if (column != 0 || row < 0 || row >= this->rowCount())
		{
			return Variant();
		}

		auto i = row;
		return items_.at(i)->getData(0, roleId);
	}

	virtual bool setData(int row, int column, ItemRole::Id roleId, const Variant& data) override
	{
		if (column != 0 || row < 0 || row >= this->rowCount())
		{
			return false;
		}

		return items_.at(row)->setData(column, roleId, data);
	}

	virtual AbstractItem* item(int row) const override
	{
		if (row >= static_cast<int>(items_.size()))
		{
			return nullptr;
		}

		return items_[row].get();
	}

	virtual int index(const AbstractItem* item) const override
	{
		auto itr = std::find_if(items_.begin(), items_.end(),
		                        [item](const std::unique_ptr<ToolsItem>& atem) { return atem.get() == item; });
		if (itr == items_.end())
		{
			return -1;
		}
		return int(itr - items_.begin());
	}

	virtual int columnCount() const override
	{
		return 1;
	}

	virtual int rowCount() const override
	{
		return static_cast<int>(items_.size());
	}

	void iterateRoles(const std::function<void(const char*)>& iterFunc) const override
	{
		for (auto&& role : ToolboxPanelDetails::s_RolesVec)
		{
			iterFunc(role.c_str());
		}
	}

	virtual std::vector<std::string> roles() const override
	{
		return ToolboxPanelDetails::s_RolesVec;
	}

	void refresh()
	{
		preModelReset_();
		items_.clear();
		if (auto toolbox = get<IToolbox>())
		{
			auto tools = toolbox->getAllTools();
			for (auto tool : tools)
			{
				items_.emplace_back(new ToolsItem(tool));
			}
		}
		postModelReset_();
	}

	void setActive(int row, bool active)
	{
		if (row < 0 || row >= static_cast<int>(items_.size()))
		{
			return;
		}

		items_[row]->setActive(active);
	}

	Connection connectPreModelReset(VoidCallback callback)
	{
		return preModelReset_.connect(callback);
	}

	Connection connectPostModelReset(VoidCallback callback)
	{
		return postModelReset_.connect(callback);
	}

private:
	Signal<VoidSignature> preModelReset_;
	Signal<VoidSignature> postModelReset_;
	std::vector<std::unique_ptr<ToolsItem>> items_;
};

class ToolboxContextObject : Depends<IDefinitionManager, IToolbox>
{
	DECLARE_REFLECTED
public:
	ToolboxContextObject() : activeToolModel_(new proto::PropertyTreeModel())
	{
	}
	~ToolboxContextObject() = default;

	void setObject()
	{
		auto definitionManager = get<IDefinitionManager>();
		TF_ASSERT(definitionManager);
		if (auto toolbox = get<IToolbox>())
		{
			auto object = toolbox->getActiveTool();
			activeToolModel_->setObject(reflectedRoot(object, *definitionManager));
		}
	}

	void refresh()
	{
		toolsModel_.refresh();
	}

	void setNotifyActive(int index, bool active)
	{
		for (int i = 0; i < toolsModel_.rowCount(); ++i)
		{
			toolsModel_.setActive(i, i == index ? active : false);
		}
	}

private:
	const AbstractListModel* tools() const
	{
		return &toolsModel_;
	}

	void activateTool(int index)
	{
		if (auto toolbox = get<IToolbox>())
		{
			auto tools = toolbox->getAllTools();
			TF_ASSERT(index >= 0 && index < static_cast<int>(tools.size()));
			if (index >= 0 && index < static_cast<int>(tools.size()))
			{
				bool isActive = tools[index] == toolbox->getActiveTool();
				toolbox->setActiveTool(isActive ? nullptr : tools[index]);
				setNotifyActive(index, !isActive);
				setObject();
			}
		}
	}

	const AbstractTreeModel* activeTool() const
	{
		if (auto toolbox = get<IToolbox>())
		{
			return activeToolModel_.get();
		}

		return nullptr;
	}

private:
	ToolsModel toolsModel_;
	std::unique_ptr<proto::PropertyTreeModel> activeToolModel_;
};
}

BEGIN_EXPOSE(ToolboxPanelDetails::ToolboxContextObject)
EXPOSE("tools", tools)
EXPOSE("activeTool", activeTool)
EXPOSE_METHOD("activateTool", activateTool)
END_EXPOSE()

BEGIN_EXPOSE(ToolboxPanelDetails::ToolObject)
EXPOSE("icon", icon)
EXPOSE("tooltip", tooltip)
EXPOSE("canActivate", canActivate, MetaSignalFunc(getCanActivateSignal))
EXPOSE("active", active, setActive, MetaSignalFunc(getActiveSignal))
END_EXPOSE()

//////////////////////////////////////////////////////////////////////////
struct ToolboxPanel::Impl
{
	ManagedObject<ToolboxPanelDetails::ToolboxContextObject> toolboxContextObject_;
	wg_future<std::unique_ptr<IView>> toolboxViewFuture_;
	std::unique_ptr<IView> toolboxView_;
	ConnectionHolder connections_;
};

ToolboxPanel::ToolboxPanel() : impl_(new Impl)
{
}

ToolboxPanel::~ToolboxPanel()
{
}

void ToolboxPanel::init()
{
	IDefinitionManager* definitionManager = get<IDefinitionManager>();

	definitionManager->registerDefinition<TypeClassDefinition<ToolboxPanelDetails::ToolObject>>();
	definitionManager->registerDefinition<TypeClassDefinition<ToolboxPanelDetails::ToolboxContextObject>>();
	impl_->toolboxContextObject_ = ManagedObject<ToolboxPanelDetails::ToolboxContextObject>::make();

	if (auto viewCreator = get<IViewCreator>())
	{
		impl_->toolboxViewFuture_ =
		viewCreator->createView("plg_toolbox/toolbox_panel.qml", impl_->toolboxContextObject_.getHandle());
	}

	impl_->toolboxContextObject_->setObject();
	impl_->toolboxContextObject_->refresh();

	if (auto toolbox = get<IToolbox>())
	{
		impl_->connections_ += toolbox->connectToolsetChanged([this]() { impl_->toolboxContextObject_->refresh(); });
		impl_->connections_ += toolbox->connectActiveToolChanged([this](ObjectHandleT<ITool> tool) {
			if (auto toolbox = get<IToolbox>())
			{
				auto tools = toolbox->getAllTools();
				auto found = std::find(tools.begin(), tools.end(), tool);
				auto index = found != tools.end() ? static_cast<int>(std::distance(tools.begin(), found)) : -1;
				impl_->toolboxContextObject_->setNotifyActive(index, true);
			}
			impl_->toolboxContextObject_->setObject();
		});
	}
}
void ToolboxPanel::fini()
{
	auto uiApplication = get<IUIApplication>();
	TF_ASSERT(uiApplication != nullptr);
	if (impl_->toolboxViewFuture_.valid())
	{
		impl_->toolboxView_ = std::move(impl_->toolboxViewFuture_.get());
	}

	if (impl_->toolboxView_)
	{
		uiApplication->removeView(*impl_->toolboxView_);
		impl_->toolboxView_ = nullptr;
	}
}

void ToolboxPanel::activate()
{
	if (impl_->toolboxViewFuture_.valid())
	{
		impl_->toolboxView_ = std::move(impl_->toolboxViewFuture_.get());
	}

	if (impl_->toolboxView_)
	{
		impl_->toolboxView_->setFocus(true);
	}
}

} // end namespace wgt
