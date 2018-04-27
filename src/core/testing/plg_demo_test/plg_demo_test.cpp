#include "core_dependency_system/i_interface.hpp"
#include "core_dependency_system/depends.hpp"

#include "core_generic_plugin/generic_plugin.hpp"
#include "core_logging/logging.hpp"
#include "core_qt_common/i_qt_framework.hpp"

#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_ui_framework/i_action.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"

#include "core_reflection/generic/generic_object.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/reflected_method_parameters.hpp"

#include "core_generic_plugin/interfaces/i_component_context.hpp"

#include "tinyxml2.hpp"

#include "wg_types/vector2.hpp"
#include "wg_types/vector3.hpp"
#include "wg_types/vector4.hpp"

#include "demo_doc.hpp"

#include <stdio.h>
#include "core_serialization/i_file_system.hpp"

#include "metadata/demo_objects.mpp"
#include "core_reflection/utilities/reflection_auto_register.hpp"

WGT_INIT_QRC_RESOURCE

namespace wgt
{
namespace
{
enum class ModelPropertyValueType : uint8_t
{
	String = 0,
	Boolean,
	Integer,
	Double,
	EnumList,
	Vector2,
	Vector3,
	Vector4,
};
}

/**
* A plugin which creates two 3D viewports with their own environments that displays sample models which can be
* interacted with.
*
* @ingroup plugins
* @image html plg_demo_test.png
* @note Requires Plugins:
*       - @ref coreplugins
*/
class DemoTestPlugin : public PluginMain, public Depends<IViewCreator>
{
private:
	std::unique_ptr<DemoDoc> demoDoc_;
	std::unique_ptr<DemoDoc> demoDoc2_;
	wg_future<std::unique_ptr<IView>> propertyView_;
	wg_future<std::unique_ptr<IView>> sceneBrowser_;
	ManagedObject<DemoObjects> demoModel_;

	std::unique_ptr<IAction> createAction_;

public:
	//==========================================================================
	DemoTestPlugin(IComponentContext& contextManager)
	{
		registerCallback([](IDefinitionManager & defManager)
		{
			ReflectionAutoRegistration::initAutoRegistration(defManager);
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
		auto uiApplication = contextManager.queryInterface<IUIApplication>();
		auto uiFramework = contextManager.queryInterface<IUIFramework>();
		auto envManager = contextManager.queryInterface<IEnvManager>();
		auto defManager = contextManager.queryInterface<IDefinitionManager>();
		if (uiApplication == nullptr || uiFramework == nullptr || envManager == nullptr || defManager == nullptr)
		{
			return;
		}

		demoModel_ = ManagedObject<DemoObjects>::make(*envManager);
		demoModel_->init(contextManager);

		auto viewCreator = get<IViewCreator>();
		if (viewCreator)
		{
			propertyView_ = viewCreator->createView("DemoTest/DemoPropertyPanel.qml", demoModel_.getHandle());
			sceneBrowser_ = viewCreator->createView("DemoTest/DemoListPanel.qml", demoModel_.getHandle());
		}

		demoDoc_.reset(new DemoDoc("sceneModel0", demoModel_.getHandleT()));
		demoDoc2_.reset(new DemoDoc("sceneModel1", demoModel_.getHandleT()));
		envManager->switchEnvironment(demoDoc_->getId());

		uiFramework->loadActionData(":/DemoTest/actions.xml", IUIFramework::ResourceType::File);

		createAction_ = uiFramework->createAction("NewObject", [this, defManager](const IAction* action) { createObject(defManager); },
		                                          [this](const IAction* action) { return canCreate(); });

		uiApplication->addAction(*createAction_);
	}

	//==========================================================================
	bool Finalise(IComponentContext& contextManager)
	{
		auto uiApplication = contextManager.queryInterface<IUIApplication>();
		if (uiApplication == nullptr)
		{
			return false;
		}
		if (propertyView_.valid())
		{
			auto view = propertyView_.get();
			uiApplication->removeView(*view);
			view = nullptr;
		}
		if (sceneBrowser_.valid())
		{
			auto view = sceneBrowser_.get();
			uiApplication->removeView(*view);
			view = nullptr;
		}
		if (createAction_ != nullptr)
		{
			uiApplication->removeAction(*createAction_);
			createAction_ = nullptr;
		}

		demoModel_->fini();
		demoDoc_ = nullptr;
		demoDoc2_ = nullptr;
		demoModel_ = nullptr;
		return true;
	}

	//==========================================================================
	void Unload(IComponentContext& contextManager)
	{
	}

private:
	void createObject(IDefinitionManager* defManager)
	{
		IClassDefinition* def = defManager->getDefinition<DemoObjects>();
		PropertyAccessor pa = def->bindProperty("newObject", demoModel_.getHandle());
		assert(pa.isValid());
		ReflectedMethodParameters parameters;
		parameters.push_back(Vector3(0.f, 0.f, -10.f));
		Variant returnValue = pa.invoke(parameters);
	}

	bool canCreate()
	{
		return true;
	}
};

PLG_CALLBACK_FUNC(DemoTestPlugin)
} // end namespace wgt
