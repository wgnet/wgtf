CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_demo_test )

INCLUDE( WGToolsCoreProject )

FIND_PACKAGE( QT5 )

IF (Qt5_WEB_ENGINE_SUPPORT)
	ADD_DEFINITIONS( -DUSE_Qt5_WEB_ENGINE=1 )
ENDIF()

SET( ALL_SRCS
	metadata/demo_objects.mpp
	plg_demo_test.cpp
	demo_objects.cpp
	demo_objects.hpp
	object_selection_helper.cpp
	object_selection_helper.hpp
	tinyxml2.cpp
	tinyxml2.hpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_QT_ADD_QRC( DemoTest resources/DemoTest )
BW_GLOB_RESOURCE_FILES()

FILE( GLOB PLUGIN_RESOURCES resources/*.xml )
BW_COPY_PLUGIN_RESOURCES( "${PLUGIN_RESOURCES}" )

BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
	${ALL_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
	${PLUGIN_RESOURCES}    
)

BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	core_reflection
	core_generic_plugin
	core_qt_common
	core_ui_framework
	Qt5::Core
	Qt5::Quick
)

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )

