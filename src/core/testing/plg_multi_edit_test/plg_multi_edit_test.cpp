#include "core_dependency_system/i_interface.hpp"
#include "core_dependency_system/depends.hpp"

#include "core_generic_plugin/interfaces/i_application.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/type_class_definition.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"

#include "chunk_item_test.hpp"
#include "multi_edit_test_data.hpp"

#include "metadata/chunk_item_test.mpp"
#include "metadata/multi_edit_test_data.mpp"
#include "core_reflection/utilities/reflection_auto_register.hpp"

#include <vector>

WGT_INIT_QRC_RESOURCE

namespace wgt
{

/**
* A plugin which creates multiple data models and merges them together.
*
* @ingroup plugins
* @note Requires Plugins:
*       - @ref coreplugins
*/
class MultiEditTestPlugin : public PluginMain, public Depends<IViewCreator>
{
private:
	InterfacePtrs types_;
	wg_future<std::unique_ptr<IView>> multiEditView_;
	wg_future<std::unique_ptr<IView>> chunkItemView_;
	ManagedObject<MultiEditTestModel> model_;
	ManagedObject<MultiEditTest::ChunkItemTestContext> chunkItemContext_;

public:
	//==========================================================================
	MultiEditTestPlugin(IComponentContext& componentContext)
	{
		registerCallback([](IDefinitionManager & defManager)
		{
			ReflectionAutoRegistration::initAutoRegistration(defManager);
		});
	}

	//==========================================================================
	bool PostLoad(IComponentContext& contextManager) override
	{
		return true;
	}

	//==========================================================================
	void Initialise(IComponentContext& contextManager) override
	{
		auto pDefinitionManager = contextManager.queryInterface<IDefinitionManager>();
		auto uiApplication = contextManager.queryInterface<IUIApplication>();
		auto uiFramework = contextManager.queryInterface<IUIFramework>();
		assert((pDefinitionManager != nullptr) && (uiFramework != nullptr) && (uiApplication != nullptr));
		auto& definitionManager = (*pDefinitionManager);

		model_ = ManagedObject<MultiEditTestModel>::make();
		model_->init(contextManager);

		chunkItemContext_ = ManagedObject<MultiEditTest::ChunkItemTestContext>::make(contextManager);

		auto viewCreator = get<IViewCreator>();
		if (viewCreator)
		{
			multiEditView_ = viewCreator->createView("plg_multi_edit_test/multi_edit_panel.qml", model_.getHandle());
			chunkItemView_ = viewCreator->createView(
				"plg_multi_edit_test/multi_edit_chunk_item_panel.qml",
				chunkItemContext_.getHandle());
		}
	}

	//==========================================================================
	bool Finalise(IComponentContext& contextManager) override
	{
		auto uiApplication = contextManager.queryInterface<IUIApplication>();
		assert(uiApplication != nullptr);
		if (chunkItemView_.valid())
		{
			auto view = chunkItemView_.get();
			uiApplication->removeView(*view);
			view = nullptr;
		}
		if (multiEditView_.valid())
		{
			auto view = multiEditView_.get();
			uiApplication->removeView(*view);
			view = nullptr;
		}

		chunkItemContext_ = nullptr;

		model_->fini();
		model_ = nullptr;

		return true;
	}
	//==========================================================================
	void Unload(IComponentContext& contextManager) override
	{
		for (auto type : types_)
		{
			contextManager.deregisterInterface(type.get());
		}
	}
};

PLG_CALLBACK_FUNC(MultiEditTestPlugin)
} // end namespace wgt
