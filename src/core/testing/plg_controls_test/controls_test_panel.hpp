#pragma once
#ifndef _CONTROLS_TEST_PANEL_HPP
#define _CONTROLS_TEST_PANEL_HPP

#include "core_ui_framework/i_view.hpp"
#include "core_generic_plugin/interfaces/i_component_context.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_object/managed_object.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_data_model/collection_model.hpp"

#include <memory>

namespace wgt
{

class AbstractListModel;

class ColorSliderDataModel
{
	DECLARE_REFLECTED
public:
	ColorSliderDataModel();
	~ColorSliderDataModel();

	AbstractListModel* getColorData() const;
	AbstractListModel* getPositionData() const;

private:
	mutable CollectionModel colorModel_;
	mutable CollectionModel positionModel_;
	mutable std::vector<Vector4> colorData_;
	mutable std::vector<int32_t> positionData_;
};

class ControlsTestPanel : Depends<IViewCreator, IUIApplication, IDefinitionManager>
{
public:
	bool addPanel();
	void removePanel();

private:
	wg_future<std::unique_ptr<IView>> controlsView_;
	std::unique_ptr<ManagedObject<ColorSliderDataModel>> controlData_;
};

} // end namespace wgt
#endif // _CONTROLS_TEST_PANEL_HPP
