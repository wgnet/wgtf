CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_qml_preferences )

INCLUDE( WGToolsCoreProject )

FIND_PACKAGE( QT5 )


SET( ALL_SRCS
	qml_preferences_panel.hpp
	qml_preferences_panel.cpp
	plg_qml_preferences.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)

BW_QT_ADD_QRC( WGQmlPreferences resources/WGQmlPreferences )
BW_GLOB_RESOURCE_FILES()

BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
	${BLOB_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	core_generic_plugin
)

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )
