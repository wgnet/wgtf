#include "core_generic_plugin/generic_plugin.hpp"

#include "core_reflection/i_definition_manager.hpp"

#include "metadata/plugin_objects.mpp"
#include "core_reflection/utilities/reflection_auto_register.hpp"

#include "plugin_objects.hpp"

namespace wgt
{
class TestPlugin2 : public PluginMain
{
public:
	TestPlugin2(IComponentContext& contextManager)
	{
		registerCallback([](IDefinitionManager & defManager)
		{
			ReflectionAutoRegistration::initAutoRegistration(defManager);
		});
	}

	bool PostLoad(IComponentContext& contextManager)
	{
		pInterface_ = TestPlugin2InterfaceObj::make();
		types_.push_back(contextManager.registerInterface(pInterface_.getPointer(), false));

		return (pInterface_ != nullptr);
	}

	void Unload(IComponentContext& contextManager)
	{
		for (auto& type : types_)
		{
			contextManager.deregisterInterface(type.get());
		}
		pInterface_ = nullptr;
	}

private:
    TestPlugin2InterfaceObj pInterface_;
	std::vector<InterfacePtr> types_;
};

PLG_CALLBACK_FUNC(TestPlugin2)
} // end namespace wgt
