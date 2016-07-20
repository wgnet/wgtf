#include "plg_node_editor.hpp"
#include "src/node_editor.hpp"

#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/reflection_macros.hpp"

#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_view.hpp"

#include "metadata/i_node_editor.mpp"
#include "metadata/i_graph.mpp"
#include "metadata/i_slot.mpp"
#include "metadata/i_connection.mpp"
#include "metadata/i_node.mpp"

#include "src/connection_curve.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"

namespace wgt
{
NodeEditorPlugin::NodeEditorPlugin(IComponentContext& context)
{}

bool NodeEditorPlugin::PostLoad(IComponentContext& context)
{
	return true;
}

void NodeEditorPlugin::Initialise(IComponentContext& context)
{
	auto definitionManager = context.queryInterface<IDefinitionManager>();

    assert(definitionManager != nullptr);

    auto uiApplication = context.queryInterface<IUIApplication>();
    auto uiFramework = context.queryInterface<IUIFramework>();

    assert(uiApplication != nullptr);
    assert(uiFramework != nullptr);

    definitionManager->registerDefinition< TypeClassDefinition< INodeEditor > >();
    definitionManager->registerDefinition< TypeClassDefinition< IGraph > >();
    definitionManager->registerDefinition< TypeClassDefinition< INode > >();
    definitionManager->registerDefinition< TypeClassDefinition< ISlot > >();
    definitionManager->registerDefinition< TypeClassDefinition< IConnection > >();
    qmlRegisterType<ConnectionCurve>("CustomConnection", 1, 0, "ConnectionCurve");

    auto nodeEditor = std::unique_ptr<INodeEditor>(new NodeEditor());
    context.registerInterface<INodeEditor>(nodeEditor.get(), false); 

    auto viewCreator = context.queryInterface<IViewCreator>();
    if (viewCreator)
    {
        view_ = viewCreator->createView(
            "plg_node_editor/NodeEditorView.qml",
            std::move(nodeEditor) );
    }
}

bool NodeEditorPlugin::Finalise(IComponentContext& context)
{
    auto uiApplication = context.queryInterface<IUIApplication>();
    assert(uiApplication != nullptr);
    if(view_.valid())
    {
        auto view = view_.get();
        uiApplication->removeView(*view);
        view.reset(nullptr);
    }

	return true;
}

void NodeEditorPlugin::Unload(IComponentContext& context)
{}
} // end namespace wgt
