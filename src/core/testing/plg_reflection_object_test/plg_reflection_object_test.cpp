#include "core_dependency_system/i_interface.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "testing/reflection_objects_test/test_objects.hpp"
#include "testing/reflection_objects_test/reflection_auto_reg.mpp"
#include "core_reflection/utilities/reflection_auto_register.hpp"
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
		registerCallback([](IDefinitionManager & definitionManager)
		{
			ReflectionAutoRegistration::initAutoRegistration(definitionManager);
		});
	}

	//==========================================================================
	bool PostLoad(IComponentContext& contextManager)
	{
		return true;
	}

	//==========================================================================
	void Initialise(IComponentContext& contextManager)
	{
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
