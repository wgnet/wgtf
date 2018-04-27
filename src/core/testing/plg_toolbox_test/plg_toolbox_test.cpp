#include "test_tool.hpp"

#include <toolbox/i_toolbox.hpp>
#include "core_generic_plugin/generic_plugin.hpp"

WGT_INIT_QRC_RESOURCE

namespace wgt
{
/**
* A plugin which creates toolbox panel with sample tools
*
* @ingroup plugins
* @note Requires Plugins:
*       - @ref coreplugins
*/
class ToolboxTestPlugin : public PluginMain, Depends<IToolbox>
{
private:
	ManagedObject<DummyTool> dummyTool_;
	ManagedObject<NovaTool> novaTool_;

public:
	//==========================================================================
	bool PostLoad(IComponentContext& contextManager)
	{
		auto definitionManager = contextManager.queryInterface<IDefinitionManager>();
		definitionManager->registerDefinition<TypeClassDefinition<BaseMode>>();
		definitionManager->registerDefinition<TypeClassDefinition<TextMode>>();
		definitionManager->registerDefinition<TypeClassDefinition<ComboBoxMode>>();
		definitionManager->registerDefinition<TypeClassDefinition<ColorMode>>();
		definitionManager->registerDefinition<TypeClassDefinition<DummyTool>>();
		definitionManager->registerDefinition<TypeClassDefinition<NovaTool>>();

		dummyTool_ = ManagedObject<DummyTool>::make();
		novaTool_ = ManagedObject<NovaTool>::make();
		return true;
	}

	//==========================================================================
	void Initialise(IComponentContext& contextManager)
	{
		if (auto toolbox = get<IToolbox>())
		{
			toolbox->registerTool(staticCast<ITool>(dummyTool_.getHandleT()));
			toolbox->registerTool(staticCast<ITool>(novaTool_.getHandleT()));
			toolbox->setActiveTool(staticCast<ITool>(novaTool_.getHandleT()));
			toolbox->setDefaultIcon("qrc:/plg_toolbox_test/icons/default.png");
		}
	}

	//==========================================================================
	bool Finalise(IComponentContext& contextManager)
	{
		return true;
	}
	//==========================================================================
	void Unload(IComponentContext& contextManager)
	{
	}
};

PLG_CALLBACK_FUNC(ToolboxTestPlugin)
} // end namespace wgt
