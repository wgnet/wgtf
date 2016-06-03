#include "controls_test_panel.hpp"
#include "core_logging/logging.hpp"
#include "core_variant/variant.hpp"
#include "core_data_model/generic_list.hpp"
#include "wg_types/vector4.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/function_property.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "core_ui_framework/i_ui_application.hpp"

namespace wgt
{
//////////////////////////////////////////////////////////////////////////
class ColorSliderDataModel
{
	DECLARE_REFLECTED
public:
	ColorSliderDataModel();
	~ColorSliderDataModel();

	IListModel * getColorData() const;
	IListModel * getPositionData() const;

private:
	mutable GenericListT< Vector4 > colorData_;
	mutable GenericListT< int32_t > positionData_;
};
BEGIN_EXPOSE(ColorSliderDataModel, MetaNoSerialization() )
	EXPOSE( "colorSource", getColorData, MetaNone() )
	EXPOSE( "positionSource", getPositionData, MetaNone() )
END_EXPOSE()

ColorSliderDataModel::ColorSliderDataModel()
{
	colorData_.emplace_back( Vector4( 127, 0, 0, 255 ) );
	colorData_.emplace_back( Vector4( 255, 0, 0, 255 ) );
	colorData_.emplace_back( Vector4( 255, 255, 0, 255 ) );
	colorData_.emplace_back( Vector4( 255, 255, 255, 255 ) );
	positionData_.emplace_back( 20 );
	positionData_.emplace_back( 40 );
	positionData_.emplace_back( 60 );
	positionData_.emplace_back( 80 );
}

ColorSliderDataModel::~ColorSliderDataModel()
{
}

IListModel * ColorSliderDataModel::getColorData() const
{
	return &colorData_;
}

IListModel * ColorSliderDataModel::getPositionData() const
{
	return &positionData_;
}


ControlsTestPanel::ControlsTestPanel( IComponentContext & context )
	: Depends( context )
{
}

//////////////////////////////////////////////////////////////////////////
bool ControlsTestPanel::addPanel()
{
	auto viewCreator = get< IViewCreator >();
	auto defManager = this->get< IDefinitionManager >();
	if ((viewCreator == nullptr) ||
		(defManager == nullptr))
	{
		return false;
	}
	IDefinitionManager& definitionManager = *defManager;
	auto def = REGISTER_DEFINITION( ColorSliderDataModel );
	controlData_ = def->create();
	viewCreator->createView(
		"WGControlsTest/WGControlsTestPanel.qml",
		controlData_, controlsView_ );
	return true;
}


void ControlsTestPanel::removePanel()
{
	auto uiApplication = this->get< IUIApplication >();
	if (uiApplication == nullptr)
	{
		return;
	}

	if (controlsView_ != nullptr)
	{
		uiApplication->removeView( *controlsView_ );
		controlsView_ = nullptr;
	}
}
} // end namespace wgt
