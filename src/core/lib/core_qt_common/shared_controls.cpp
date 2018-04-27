#include "shared_controls.hpp"

#include "register_type.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_dependency_system/i_interface.hpp"

#include <QtQuick>

#include "qml_control_list.inl"

namespace wgt
{
void SharedControls::init()
{
	registerQmlTypes();
}

} // end namespace wgt
