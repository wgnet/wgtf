CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_panel_manager )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )
find_package(QT5 ${QT_FIND_PACKAGE_VERSION})


SET( ALL_SRCS
	${DISPLAY}
	${DISPLAY_METADATA}
	panel_manager.cpp
	panel_manager.hpp
	plg_panel_manager.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_QT_ADD_QRC(plg_panel_manager resources/plg_panel_manager)
BW_GLOB_RESOURCE_FILES()

WG_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)

BW_ADD_TOOL_PLUGIN( plg_panel_manager SHARED
	${BLOB_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( plg_panel_manager PRIVATE
	core_generic_plugin
	core_data_model
	core_reflection
)

BW_PROJECT_CATEGORY( plg_panel_manager "Plugins" )
