// Declaration of PluginMain
#include "core_generic_plugin/generic_plugin.hpp"

// Declaration of the Panel
#include "qml_preferences_panel.hpp"

#include "core_reflection/type_class_definition.hpp"

#include <memory>

WGT_INIT_QRC_RESOURCE

namespace wgt
{
/**
* A plugin which creates a preferences option in the menu bar that lets you select different UI colours
*
* @ingroup plugins
* @image html plg_qml_preferences.png
* @note Requires Plugins:
*       - @ref coreplugins
*       - ColorPickerPlugin
*/
class QmlPreferencesPanelPlugin : public PluginMain
{
public:
	QmlPreferencesPanelPlugin(IComponentContext& componentContext)
	{
	}

	bool PostLoad(IComponentContext& componentContext) override
	{
		// Create the panel
		qmlPreferencesPanel_.reset(new QmlPreferencesPanel(componentContext));
		return true;
	}

	void Initialise(IComponentContext& componentContext) override
	{
		qmlPreferencesPanel_->addPanel();
	}

	bool Finalise(IComponentContext& componentContext) override
	{
		qmlPreferencesPanel_->removePanel();
		return true;
	}

	void Unload(IComponentContext& componentContext) override
	{
		qmlPreferencesPanel_.reset();
	}

private:
	std::unique_ptr<QmlPreferencesPanel> qmlPreferencesPanel_;
};

PLG_CALLBACK_FUNC(QmlPreferencesPanelPlugin)
} // end namespace wgt
