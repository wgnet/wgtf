#include "qt_sharable.hpp"

#include "wg_context_menu_simple.hpp"

#include "core_qt_common/qt_scripting_engine_base.hpp"
#include "core_wgtf_app/app_common.cpp"
#include "qml_control_list.inl"

namespace wgt
{
namespace AppCommonPrivate
{
//TODO: make qt_sharable independent from dependency manager.
void staticInitPlugin()
{
}
}
}

using namespace wgt;

void QtSharable::registerTypes(const char* uri)
{
	registerQmlTypes();
	qmlRegisterType<WGContextMenuSimple>("WGControls", 1, 0, "WGContextMenu");
	qmlRegisterType<WGContextMenuSimple>("WGControls", 2, 0, "WGContextMenu");
}

void QtSharable::initTF(std::unique_ptr<QQmlEngine> qmlEngine)
{
	registerTypes("WGControls");
	qmlEngine->setParent(nullptr);
	qtFramework = std::unique_ptr<wgt::QtFrameworkCommon>(new wgt::QtFrameworkCommon(
	std::move(qmlEngine), std::unique_ptr<QtScriptingEngineBase>(new QtScriptingEngineBase())));
	qtFramework->initialise();
}

void QtSharable::finalizeTF()
{
	qtFramework->finalise();
}
