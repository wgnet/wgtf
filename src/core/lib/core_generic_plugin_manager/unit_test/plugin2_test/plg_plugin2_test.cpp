#include "core_generic_plugin/generic_plugin.hpp"

#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/object_manager.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/reflected_types.hpp"
#include "core_reflection/metadata/meta_types.hpp"

#include "plugin_objects.hpp"

namespace wgt
{
class TestPlugin2 : public PluginMain
{
public:
	TestPlugin2(IComponentContext& contextManager)
	{
	}

	bool PostLoad(IComponentContext& contextManager)
	{
		IDefinitionManager* pDefinitionManager = contextManager.queryInterface<IDefinitionManager>();
		if (pDefinitionManager == nullptr)
		{
			return false;
		}

		IDefinitionManager& definitionManager = (*pDefinitionManager);
		REGISTER_DEFINITION(TestPlugin2TestObject);
		REGISTER_DEFINITION(TestPlugin2Interface);

		pInterface_ = definitionManager.create<TestPlugin2Interface>();
		types_.push_back(contextManager.registerInterface(pInterface_.get(), false));

		return (pInterface_ != nullptr);
	}

	void Unload(IComponentContext& contextManager)
	{
		for (auto type : types_)
		{
			contextManager.deregisterInterface(type);
		}
		pInterface_ = nullptr;
	}

private:
	ObjectHandleT<TestPlugin2Interface> pInterface_;
	std::vector<IInterface*> types_;
};

PLG_CALLBACK_FUNC(TestPlugin2)
} // end namespace wgt
