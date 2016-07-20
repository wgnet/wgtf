
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
* A plugin which creates a panel with a color picker dialog. 
* The color picker allows the colour to be manipulated by a wheel, sliders or picked from the screen.
*
* @ingroup plugins
* @image html plg_color_picker.png 
* @note Requires Plugins:
*       - @ref coreplugins
*/
class ColorPickerPlugin
    : public PluginMain
{
public:
    ColorPickerPlugin( IComponentContext & componentContext )
    {
    }
 
    bool PostLoad( IComponentContext & componentContext ) override
    {
		auto defManager = componentContext.queryInterface< IDefinitionManager >();
		defManager->registerDefinition<TypeClassDefinition< ColorPickerContext >>();

        // Create the panel
        colorPicker_.reset( new ColorPicker( componentContext ) );
        return true;
    }
 
    void Initialise( IComponentContext & componentContext ) override
    {
        colorPicker_->addPanel();
    }
 
    bool Finalise( IComponentContext & componentContext ) override
    {
        colorPicker_->removePanel();
        return true;
    }
 
    void Unload( IComponentContext & componentContext ) override
    {
        colorPicker_.reset();
    }
 
private:
    std::unique_ptr< ColorPicker > colorPicker_;
};
 
 
PLG_CALLBACK_FUNC( ColorPickerPlugin )
} // end namespace wgt
