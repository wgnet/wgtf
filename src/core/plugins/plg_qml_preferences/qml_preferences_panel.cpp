#include "qml_preferences_panel.hpp"
#include "core_logging/logging.hpp"
#include "core_variant/variant.hpp"

namespace wgt
{
QmlPreferencesPanel::QmlPreferencesPanel( IComponentContext & context )
	: Depends( context )
{
}


bool QmlPreferencesPanel::addPanel()
{

	auto uiFramework = this->get< IUIFramework >();
	auto uiApplication = this->get< IUIApplication >();

	if ((uiFramework == nullptr) ||
		(uiApplication == nullptr))
	{
		return false;
	}


	auto viewCreator = get< IViewCreator >();
	if (viewCreator)
	{
		qmlPreferencesView_ = viewCreator->createView(
			"WGQmlPreferences/QmlPreferencesPanel.qml",
			ObjectHandle());
	}

	return true;
}


void QmlPreferencesPanel::removePanel()
{
	auto uiApplication = this->get< IUIApplication >();
	if (uiApplication == nullptr)
	{
		return;
	}

	if (qmlPreferencesView_.valid())
	{
		auto view = qmlPreferencesView_.get();
		uiApplication->removeView( *view );
	}
}
}
