#include <memory>
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_reflection/type_class_definition.hpp"
#include "core_reflection/interfaces/i_definition_helper.hpp"
#include "core_variant/default_meta_type_manager.hpp"

#include "plugins/plg_node_editor/interfaces/i_node_editor.hpp"
#include "implements/CustomGraph.h"


namespace wgt
{
class NodeEditorTestPlugin
    : public PluginMain
{
public:
    NodeEditorTestPlugin(IComponentContext & contextManager) {}

    void Initialise(IComponentContext & contextManager) override
    {
        auto metaTypeManager = contextManager.queryInterface<IMetaTypeManager>();
        auto nodeEditor = contextManager.queryInterface<INodeEditor>();
        assert(nodeEditor != nullptr);
        assert(metaTypeManager != nullptr);

        Variant::setMetaTypeManager(metaTypeManager);

        std::shared_ptr<IGraph> graph(new CustomGraph());
        nodeEditor->SetGraph(graph);
    }
};

PLG_CALLBACK_FUNC(NodeEditorTestPlugin)
} // end namespace wgt
