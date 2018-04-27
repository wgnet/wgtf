#include "plg_node_editor.hpp"
#include "src/node_editor.hpp"
#include "src/group.hpp"

#include "core_common/assert.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/reflection_macros.hpp"

#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_view.hpp"

#include "core_ui_framework/interfaces/i_view_creator.hpp"

#include "reflection_auto_reg.mpp"
#include "core_reflection/utilities/reflection_auto_register.hpp"

WGT_INIT_QRC_RESOURCE

namespace wgt
{

BEGIN_EXPOSE(Group, IGroup, MetaNone())
END_EXPOSE()

//------------------------------------------------------------------------------
NodeEditorPlugin::NodeEditorPlugin()
{
	registerCallback([]( IDefinitionManager & defManager )
	{
		ReflectionAutoRegistration::initAutoRegistration(defManager);
	});
}

//------------------------------------------------------------------------------
bool NodeEditorPlugin::PostLoad(IComponentContext& context)
{
	return true;
}

void NodeEditorPlugin::Initialise(IComponentContext& context)
{
	auto uiApplication = context.queryInterface<IUIApplication>();
	auto uiFramework = context.queryInterface<IUIFramework>();

	TF_ASSERT(uiApplication != nullptr);
	TF_ASSERT(uiFramework != nullptr);

	uiFramework->loadActionData(":/plg_node_editor/actions.xml", IUIFramework::ResourceType::File);

	nodeEditor_ = ManagedObject<NodeEditor>::make();
    types_.push_back(context.registerInterface<INodeEditor>(nodeEditor_.getPointer(), false));

	auto viewCreator = context.queryInterface<IViewCreator>();
	if (viewCreator)
	{
		view_ = viewCreator->createView("plg_node_editor/NodeEditorView.qml", nodeEditor_.getHandleT());
	}
}

bool NodeEditorPlugin::Finalise(IComponentContext& context)
{
	auto uiApplication = context.queryInterface<IUIApplication>();
	TF_ASSERT(uiApplication != nullptr);
	if (view_.valid())
	{
		auto view = view_.get();
		uiApplication->removeView(*view);
		view.reset(nullptr);
	}

	return true;
}

void NodeEditorPlugin::Unload(IComponentContext& context)
{
    for (auto type : types_)
    {
        context.deregisterInterface(type.get());
    }
    nodeEditor_ = nullptr;
}

} // end namespace wgt
