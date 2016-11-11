#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "testing/reflection_objects_test/test_objects.hpp"
#include <vector>

namespace wgt
{
/**
* A plugin which tests the reflection plugin
*
* @ingroup plugins
* @note Requires Plugins:
*       - @ref coreplugins
*/
class ReflectionObjectTestPlugin : public PluginMain
{
private:
public:
	//==========================================================================
	ReflectionObjectTestPlugin(IComponentContext& contextManager)
	{
	}

	//==========================================================================
	bool PostLoad(IComponentContext& contextManager)
	{
		return true;
	}

	//==========================================================================
	void Initialise(IComponentContext& contextManager)
	{
		// register reflected type definition
		IDefinitionManager* defManager = contextManager.queryInterface<IDefinitionManager>();
		assert(defManager != nullptr);

		TestObjects testObjects;
		testObjects.initDefs(*defManager);
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

PLG_CALLBACK_FUNC(ReflectionObjectTestPlugin)
} // end namespace wgt
