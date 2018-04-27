#include "core_generic_plugin/interfaces/i_component_context.hpp"

#include "core_generic_plugin/generic_plugin.hpp"

#include "core_common/assert.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/i_object_manager.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/metadata/meta_types.hpp"

#include "metadata/plugin_objects.mpp"
#include "core_reflection/utilities/reflection_auto_register.hpp"

#include "plugin_objects.hpp"

namespace wgt
{
class TestPlugin1 : public PluginMain
{
public:
	TestPlugin1(IComponentContext& contextManager) : PluginMain()
	{
		registerCallback([]( IDefinitionManager & defManager )
		{
			ReflectionAutoRegistration::initAutoRegistration(defManager);
		});
	}

	bool PostLoad(IComponentContext& contextManager)
	{
        IObjectManager* pObjectManager = contextManager.queryInterface<IObjectManager>();
        if (pObjectManager == nullptr)
        {
            return false;
        }
        IDefinitionManager* pDefinitionManager = contextManager.queryInterface<IDefinitionManager>();
        if (pDefinitionManager == nullptr)
        {
            return false;
        }

        // Create plugin interface
        std::vector<RefObjectId> objects;
        pObjectManager->getContextObjects(objects);

        // Search for the plugin interface
        for (std::vector<RefObjectId>::iterator it = objects.begin(), end = objects.end(); it != end; ++it)
        {
            auto pObj = pObjectManager->getObject(*it);
            TF_ASSERT(pObj != nullptr);
            if (pDefinitionManager->getDefinition(pObj) == pDefinitionManager->getDefinition<TestPlugin1Interface>())
            {
                pInterface_ = safeCast<TestPlugin1Interface>(pObj);
                return true;
            }
        }

        // create new interface object
        pObject_ = TestPlugin1InterfaceObj::make();
        pInterface_ = pObject_.getHandleT();
        TF_ASSERT(pInterface_ != nullptr);

        // Do not delete when interface is unregistered
        // Reflection system needs to delete it
        types_.push_back(contextManager.registerInterface(pInterface_.get(), false /*transferOwnership*/));

		return true;
	}

	void Initialise(IComponentContext& contextManager)
	{
		TF_ASSERT(pInterface_ != nullptr);
		auto defManager = contextManager.queryInterface<IDefinitionManager>();
		TF_ASSERT(defManager != nullptr);
        
		ITestPlugin2* plugin2 = contextManager.queryInterface<ITestPlugin2>();
		if (plugin2 != nullptr)
		{
			pInterface_->setObjectFromPlugin2(plugin2->getObject());
		}
	}

	bool Finalise(IComponentContext& contextManager)
	{
		if (pInterface_ != nullptr)
		{
			pInterface_->setObjectFromPlugin2(nullptr);
		}
		return true;
	}

	void Unload(IComponentContext& contextManager)
	{
		for (auto type : types_)
		{
			contextManager.deregisterInterface(type.get());
		}
		pInterface_ = nullptr;
        pObject_ = nullptr;
	}

private:
    TestPlugin1InterfacePtr pInterface_;
    TestPlugin1InterfaceObj pObject_;
	InterfacePtrs types_;
};

PLG_CALLBACK_FUNC(TestPlugin1)
} // end namespace wgt
