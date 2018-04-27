CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )

PROJECT( plg_hotloading_test )
INCLUDE( WGToolsCoreProject )
FIND_PACKAGE( QT5 ${QT_FIND_PACKAGE_VERSION} )

SET( ALL_SRCS
    plg_hotloading_test.cpp
    hotloading_panel.hpp
    hotloading_panel.cpp
    metadata/hotloading_panel.mpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )
 
FILE( GLOB PLUGIN_RESOURCES resources/*.* )
BW_COPY_PLUGIN_RESOURCES( "${PLUGIN_RESOURCES}" )
 
BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
    ${ALL_SRCS}
    ${RESOURCE_FILES}
    ${COMPILED_RESOURCES}
	${PLUGIN_RESOURCES}
)
 
BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
    core_generic_plugin
    core_qt_common
    core_ui_framework
    Qt5::Core
	Qt5::Gui
	Qt5::Quick
	Qt5::QuickWidgets
)

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )
