#include "controls_test_panel.hpp"
#include "core_logging/logging.hpp"
#include "core_variant/variant.hpp"
#include "wg_types/vector4.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "core_ui_framework/i_ui_application.hpp"

namespace wgt
{
//////////////////////////////////////////////////////////////////////////
ColorSliderDataModel::ColorSliderDataModel()
{
	colorData_.emplace_back(Vector4(127, 0, 0, 255));
	colorData_.emplace_back(Vector4(255, 0, 0, 255));
	colorData_.emplace_back(Vector4(255, 255, 0, 255));
	colorData_.emplace_back(Vector4(255, 255, 255, 255));
	positionData_.emplace_back(20);
	positionData_.emplace_back(40);
	positionData_.emplace_back(60);
	positionData_.emplace_back(80);
	colorModel_.setSource(Collection(colorData_));
	positionModel_.setSource(Collection(positionData_));
}

ColorSliderDataModel::~ColorSliderDataModel()
{
}

AbstractListModel* ColorSliderDataModel::getColorData() const
{
	return &colorModel_;
}

AbstractListModel* ColorSliderDataModel::getPositionData() const
{
	return &positionModel_;
}

//////////////////////////////////////////////////////////////////////////
bool ControlsTestPanel::addPanel()
{
	auto viewCreator = get<IViewCreator>();
	if (viewCreator == nullptr)
	{
		return false;
	}
	controlData_ = ManagedObject<ColorSliderDataModel>::make_unique();
	controlsView_ = viewCreator->createView("WGControlsTest/WGControlsTestPanel.qml", controlData_->getHandle());
	return true;
}

void ControlsTestPanel::removePanel()
{
	auto uiApplication = this->get<IUIApplication>();
	if (uiApplication == nullptr)
	{
		return;
	}

	if (controlsView_.valid())
	{
		auto view = controlsView_.get();
		uiApplication->removeView(*view);
		view = nullptr;
	}

	controlData_.reset();
}
} // end namespace wgt
