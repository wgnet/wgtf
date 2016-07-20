#include <memory>
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_reflection/type_class_definition.hpp"
#include "core_reflection/interfaces/i_definition_helper.hpp"

#include "plugins/plg_node_editor/interfaces/i_node_editor.hpp"
#include "implements/CustomGraph.h"


namespace wgt
{
/**
* A plugin which queries for an INodeEditor interface to set an IGraph and add test data for manipulating nodes.
* Nodes can be created and linked to each other by right mouse clicking.
*
* @ingroup plugins
* @image html plg_node_editor.png 
* @note Requires Plugins:
*       - @ref coreplugins
*       - NodeEditorPlugin
*/
class NodeEditorTestPlugin
    : public PluginMain
{
public:
    NodeEditorTestPlugin(IComponentContext & contextManager) {}

    void Initialise(IComponentContext & contextManager) override
    {
        auto nodeEditor = contextManager.queryInterface<INodeEditor>();
        assert(nodeEditor != nullptr);

        std::shared_ptr<IGraph> graph(new CustomGraph());
        nodeEditor->SetGraph(graph);
    }
};

PLG_CALLBACK_FUNC(NodeEditorTestPlugin)
} // end namespace wgt
