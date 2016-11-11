#include "python_panel.hpp"
#include "core_logging/logging.hpp"
#include "core_reflection/reflected_object.hpp"
#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/function_property.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"
#include "core_reflection/property_accessor_listener.hpp"
#include "core_logging/logging.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_ui_framework/i_ui_application.hpp"
#include "core_ui_framework/interfaces/i_view_creator.hpp"

namespace wgt
{
PythonPanel::PythonPanel(IComponentContext& context, ObjectHandle& contextObject)
    : Depends(context), context_(context), contextObject_(contextObject)
{
	this->addPanel();
}

PythonPanel::~PythonPanel()
{
	this->removePanel();
}

bool PythonPanel::addPanel()
{
	auto viewCreator = get<IViewCreator>();
	if (viewCreator == nullptr)
	{
		NGT_ERROR_MSG("Failed to find IViewCreator\n");
		return false;
	}
	pythonView_ = viewCreator->createView("Python27UITest/PythonObjectTestPanel.qml", contextObject_);
	return true;
}

void PythonPanel::removePanel()
{
	auto uiApplication = get<IUIApplication>();

	if (uiApplication == nullptr)
	{
		NGT_ERROR_MSG("Failed to find IUIApplication\n");
		return;
	}

	if (pythonView_.valid())
	{
		auto view = pythonView_.get();
		uiApplication->removeView(*view);
		view = nullptr;
	}
}
} // end namespace wgt
