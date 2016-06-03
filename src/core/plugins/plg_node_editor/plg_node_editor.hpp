#ifndef __PLG_NODE_EDITOR_H__
#define __PLG_NODE_EDITOR_H__

#include <memory>
#include "core_generic_plugin/generic_plugin.hpp"

namespace wgt
{
class IView;
class IComponentContext;

class NodeEditorPlugin : public PluginMain
{
public:
	NodeEditorPlugin(IComponentContext& context);
	bool PostLoad(IComponentContext& context) override;
	void Initialise(IComponentContext& context) override;
	bool Finalise(IComponentContext& context) override;
	void Unload(IComponentContext& context) override;

private:
	std::unique_ptr<IView> view;
};

PLG_CALLBACK_FUNC(NodeEditorPlugin)
} // end namespace wgt
#endif // __PLG_NODE_EDITOR_H__
