// Declaration of PluginMain
#include "core_generic_plugin/generic_plugin.hpp"

// Declaration of the Panel
#include "custom_ui_themes_dialog.hpp"

#include "core_reflection/type_class_definition.hpp"

#include <memory>

WGT_INIT_QRC_RESOURCE

namespace wgt
{
/**
* A plugin which creates a preferences option in the menu bar that lets you select different UI colours
*
* @ingroup plugins
* @image html plg_custom_ui_themes.png
* @note Requires Plugins:
*       - @ref coreplugins
*       - ColorPickerPlugin
*/
class CustomUIThemesPlugin : public PluginMain
{
public:
	CustomUIThemesPlugin(IComponentContext& componentContext)
	{
	}

	bool PostLoad(IComponentContext& componentContext) override
	{
		// Create the panel
		customUIThemesDialog_.reset(new CustomUIThemesDialog);
		return true;
	}

	void Initialise(IComponentContext& componentContext) override
	{
		customUIThemesDialog_->createDialog();
	}

	bool Finalise(IComponentContext& componentContext) override
	{
		customUIThemesDialog_->destroyDialog();
		return true;
	}

	void Unload(IComponentContext& componentContext) override
	{
		customUIThemesDialog_.reset();
	}

private:
	std::unique_ptr<CustomUIThemesDialog> customUIThemesDialog_;
};

PLG_CALLBACK_FUNC(CustomUIThemesPlugin)
} // end namespace wgt
