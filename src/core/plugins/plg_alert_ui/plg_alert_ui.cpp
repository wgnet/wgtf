#include "core_generic_plugin/generic_plugin.hpp"

#include "core_common/assert.hpp"
#include "core_generic_plugin_manager/generic_plugin_manager.hpp"
#include "core_logging_system/alerts/alert_manager.hpp"
#include "core_logging_system/interfaces/i_logging_system.hpp"
#include "core_logging_system/log_level.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/type_class_definition.hpp"
#include "core_variant/variant.hpp"
#include "alert_models.hpp"
#include "popup_alert_presenter.hpp"
#include <vector>

WGT_INIT_QRC_RESOURCE

namespace wgt
{
/**
* A plugin used to register the basic alert logger so that it can
* be used by applications to display alerts. Requires the logging system to
* also be included in the project. No alerts will be displayed unless an
* IAlertPresenter is registered with the AlertManager and an ILogger invokes
* the add() functionality on the AlertManager.
*
* @ingroup plugins
* @image html plg_alert_ui.png
* @note Requires Plugins:
*       - @ref coreplugins
*       - LoggingSystemPlugin
*/
class AlertUIPlugin : public PluginMain
{
public:
	AlertUIPlugin(IComponentContext& contextManager) : popupAlertPresenter_(nullptr)
	{
	}

	bool PostLoad(IComponentContext& contextManager) override
	{
		auto definitionManager = contextManager.queryInterface<IDefinitionManager>();
		TF_ASSERT(definitionManager != nullptr);

		definitionManager->registerDefinition<TypeClassDefinition<AlertPageModel>>();
		definitionManager->registerDefinition<TypeClassDefinition<AlertObjectModel>>();

		return true;
	}

	void Initialise(IComponentContext& contextManager) override
	{
		ILoggingSystem* loggingSystem = contextManager.queryInterface<ILoggingSystem>();
		if (loggingSystem != nullptr)
		{
			AlertManager* alertManager = loggingSystem->getAlertManager();
			if (alertManager != nullptr)
			{
				popupAlertPresenter_ = new PopupAlertPresenter(contextManager);
				alertManager->registerPresenter(popupAlertPresenter_);

				loggingSystem->enableAlertManagement();
			}
		}
	}

	bool Finalise(IComponentContext& contextManager) override
	{
		ILoggingSystem* loggingSystem = contextManager.queryInterface<ILoggingSystem>();
		if (loggingSystem != nullptr)
		{
			AlertManager* alertManager = loggingSystem->getAlertManager();
			if (alertManager != nullptr)
			{
				alertManager->unregisterPresenter(popupAlertPresenter_);
				delete popupAlertPresenter_;
				popupAlertPresenter_ = nullptr;
			}

			loggingSystem->disableAlertManagement();
		}

		return true;
	}

	void Unload(IComponentContext& contextManager) override
	{
		for (auto type : types_)
		{
			contextManager.deregisterInterface(type.get());
		}
	}

private:
	InterfacePtrs types_;

	PopupAlertPresenter* popupAlertPresenter_;
};

PLG_CALLBACK_FUNC(AlertUIPlugin)
} // end namespace wgt
