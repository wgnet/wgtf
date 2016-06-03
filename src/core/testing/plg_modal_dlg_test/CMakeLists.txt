CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_modal_dlg_test )

ADD_DEFINITIONS( -DNOMINMAX )

INCLUDE( WGToolsCoreProject )

find_package(QT5)

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS(
	-D_CRT_SECURE_NO_WARNINGS
)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( ALL_SRCS
	plg_modal_dlg_test.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_QT_ADD_QRC(plg_modal_dlg_test resources/plg_modal_dlg_test)
BW_GLOB_RESOURCE_FILES()

BW_ADD_TOOL_PLUGIN( plg_modal_dlg_test SHARED
	${ALL_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( plg_modal_dlg_test PRIVATE
	core_generic_plugin
	core_qt_common
	core_ui_framework
    Qt5::Core
)

BW_PROJECT_CATEGORY( plg_modal_dlg_test "Plugins" )
