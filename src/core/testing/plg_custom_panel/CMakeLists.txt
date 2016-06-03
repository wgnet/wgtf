CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_custom_panel )

INCLUDE( WGToolsCoreProject )

FIND_PACKAGE( QT5 )


SET( ALL_SRCS
	custom_panel.hpp
	custom_panel.cpp
	plg_custom_panel_main.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)

BW_QT_ADD_QRC( ${PROJECT_NAME} resources/plg_custom_panel )
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
