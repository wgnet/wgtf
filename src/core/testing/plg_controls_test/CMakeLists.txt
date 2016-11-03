CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_controls_test )

INCLUDE( WGToolsCoreProject )

FIND_PACKAGE( QT5 )


SET( ALL_SRCS
	controls_test_panel.hpp
	controls_test_panel.cpp
	plg_controls_test.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)

BW_QT_ADD_QRC( WGControlsTest resources/plg_controls_test )
BW_GLOB_RESOURCE_FILES()

BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
	${BLOB_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	core_generic_plugin
	core_qt_common
	core_ui_framework
	Qt5::Core
)

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )
