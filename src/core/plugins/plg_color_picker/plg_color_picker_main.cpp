
// Declaration of PluginMain
#include "core_generic_plugin/generic_plugin.hpp"
 
// Declaration of the Panel
#include "color_picker.hpp"
 
// Declaration of the type system
#include "core_variant/variant.hpp"


#include "core_reflection/type_class_definition.hpp"
#include "color_picker_context.hpp"
#include "metadata/color_picker_context.mpp" 
 
#include <memory>
 
 
namespace wgt
{
/**
 * ColorPickerPlugin
 *
 * This is the main application's plugin. It creates the temporary panel to display the ColorPIcker and
 * registers it with the UIFramework. So that it will be added to the main dialog
 */
class ColorPickerPlugin
    : public PluginMain
{
public:
    ColorPickerPlugin( IComponentContext & componentContext )
    {
    }
 
    // Plugin creates resources
    bool PostLoad( IComponentContext & componentContext ) override
    {
        // Static variable that must be set for every plugin
        Variant::setMetaTypeManager(
            componentContext.queryInterface< IMetaTypeManager >() );
 
		auto defManager = componentContext.queryInterface< IDefinitionManager >();
		defManager->registerDefinition<TypeClassDefinition< ColorPickerContext >>();

        // Create the panel
        colorPicker_.reset( new ColorPicker( componentContext ) );
        return true;
    }
 
    // Registration of services this plugin provides
    // IComponentContext provides access to services provided by other plugins
    void Initialise( IComponentContext & componentContext ) override
    {
        colorPicker_->addPanel();
    }
 
    // De-registration of services this plugin provides
    bool Finalise( IComponentContext & componentContext ) override
    {
        colorPicker_->removePanel();
        return true;
    }
 
    // Plugin deletes resources
    void Unload( IComponentContext & componentContext ) override
    {
        colorPicker_.reset();
    }
 
private:
    std::unique_ptr< ColorPicker > colorPicker_;
};
 
 
PLG_CALLBACK_FUNC( ColorPickerPlugin )
} // end namespace wgt
