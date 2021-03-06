#include "logging_view_model.hpp"
#include "logging_data_model.hpp"
#include "core_logging_system/interfaces/i_logging_system.hpp"

#include "core_common/assert.hpp"
#include "core_generic_plugin/generic_plugin.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_object/managed_object.hpp"

WGT_INIT_QRC_RESOURCE

namespace wgt
{
/**
* A plugin which registers an ILogger with the ILoggingSystem to allow logging to a panel
*
* @ingroup plugins
* @image html plg_logging_ui.png
* @note Requires Plugins:
*       - @ref coreplugins
*       - LoggingSystemPlugin
*/
class LoggingPanelPlugin : public PluginMain
{
public:
	LoggingPanelPlugin(IComponentContext& componentContext)
	{
	}

	bool PostLoad(IComponentContext& componentContext) override
	{
		return true;
	}

	void Initialise(IComponentContext& componentContext) override
	{
		auto uiFramework = componentContext.queryInterface<IUIFramework>();
		auto uiApplication = componentContext.queryInterface<IUIApplication>();
		auto defManager = componentContext.queryInterface<IDefinitionManager>();

		if ((uiFramework == nullptr) || (uiApplication == nullptr) || (defManager == nullptr))
		{
			return;
		}

		defManager->registerDefinition<TypeClassDefinition<LoggingViewModel>>();
        defManager->registerDefinition<TypeClassDefinition<LoggingDataModel>>();

		viewModel_ = ManagedObject<LoggingViewModel>::make();
		TF_ASSERT(viewModel_ != nullptr);
		bool success = viewModel_->initialise(componentContext);
		TF_ASSERT(success);

		logger_.reset(new Logger(viewModel_->getModel()));

		ILoggingSystem* loggingSystem = componentContext.queryInterface<ILoggingSystem>();
		if (loggingSystem != nullptr)
		{
			loggingSystem->registerLogger(logger_.get());
		}

		loggingView_ =
		uiFramework->createView("plg_logging_ui/logging_panel.qml", IUIFramework::ResourceType::Url, viewModel_.getHandleT());
		uiApplication->addView(*loggingView_);
	}

	bool Finalise(IComponentContext& componentContext) override
	{
		auto uiApplication = componentContext.queryInterface<IUIApplication>();
		if (uiApplication == nullptr)
		{
			return false;
		}

		uiApplication->removeView(*loggingView_);
		loggingView_.reset();
        viewModel_ = nullptr;

		return true;
	}

	void Unload(IComponentContext& componentContext) override
	{
		logger_.reset();
	}

private:
	std::unique_ptr<IView> loggingView_;
	std::unique_ptr<Logger> logger_;
    ManagedObject<LoggingViewModel> viewModel_;
};

PLG_CALLBACK_FUNC(LoggingPanelPlugin)
} // end namespace wgt