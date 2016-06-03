#include "plg_node_editor.hpp"
#include "src/node_editor.hpp"

#include "core_variant/variant.hpp"

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
	auto metaTypeManager = context.queryInterface<IMetaTypeManager>();
	auto definitionManager = context.queryInterface<IDefinitionManager>();

    assert(metaTypeManager != nullptr);
    assert(definitionManager != nullptr);

    auto uiApplication = context.queryInterface<IUIApplication>();
    auto uiFramework = context.queryInterface<IUIFramework>();

    assert(uiApplication != nullptr);
    assert(uiFramework != nullptr);

    Variant::setMetaTypeManager(metaTypeManager);
    definitionManager->registerDefinition< TypeClassDefinition< INodeEditor > >();
    definitionManager->registerDefinition< TypeClassDefinition< IGraph > >();
    definitionManager->registerDefinition< TypeClassDefinition< INode > >();
    definitionManager->registerDefinition< TypeClassDefinition< ISlot > >();
    definitionManager->registerDefinition< TypeClassDefinition< IConnection > >();
    qmlRegisterType<ConnectionCurve>("CustomConnection", 1, 0, "ConnectionCurve");

    auto nodeEditor = std::unique_ptr<INodeEditor>(new NodeEditor());
    context.registerInterface<INodeEditor>(nodeEditor.get(), false);   

    view = uiFramework->createView("plg_node_editor/NodeEditorView.qml",
        IUIFramework::ResourceType::Url, std::move(nodeEditor));
    uiApplication->addView(*view);
}

bool NodeEditorPlugin::Finalise(IComponentContext& context)
{
    auto uiApplication = context.queryInterface<IUIApplication>();
    assert(uiApplication != nullptr);

    uiApplication->removeView(*view);
    view.reset(nullptr);

	return true;
}

void NodeEditorPlugin::Unload(IComponentContext& context)
{}
} // end namespace wgt
