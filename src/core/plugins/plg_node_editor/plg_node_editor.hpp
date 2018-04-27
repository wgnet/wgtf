#ifndef __PLG_NODE_EDITOR_H__
#define __PLG_NODE_EDITOR_H__

#include <memory>
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_common/wg_future.hpp"
#include "core_object/managed_object.hpp"

namespace wgt
{
class IView;
class IComponentContext;
class NodeEditor;

/**
* A plugin which registers an INodeEditor interface and panel which can be used to create, remove and manipulate nodes
*
* @ingroup plugins
* @image html plg_node_editor.png
* @note Requires Plugins:
*       - @ref coreplugins
*/
class NodeEditorPlugin : public PluginMain
{
public:
	NodeEditorPlugin();
	bool PostLoad(IComponentContext& context) override;
	void Initialise(IComponentContext& context) override;
	bool Finalise(IComponentContext& context) override;
	void Unload(IComponentContext& context) override;

private:
	wg_future<std::unique_ptr<IView>> view_;
    ManagedObject<NodeEditor> nodeEditor_;
    InterfacePtrs types_;
};

PLG_CALLBACK_FUNC(NodeEditorPlugin)
} // end namespace wgt
#endif // __PLG_NODE_EDITOR_H__
