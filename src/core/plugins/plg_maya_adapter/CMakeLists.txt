CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_maya_adapter )

INCLUDE( WGToolsCoreProject )
find_package(QT5 ${QT_FIND_PACKAGE_VERSION})

SET( PLUGIN_QT_COMMON_DIR ${CMAKE_CURRENT_LIST_DIR}/../plg_qt_common )
SET( MAYA_APP_PLUGIN_DIR ${CMAKE_CURRENT_LIST_DIR}/../../app/maya_plugin )

INCLUDE_DIRECTORIES( ${PLUGIN_QT_COMMON_DIR} )
INCLUDE_DIRECTORIES( ${MAYA_APP_PLUGIN_DIR}/app/interfaces )

BW_QT_ADD_QRC(WGControls ${PLUGIN_QT_COMMON_DIR}/resources/WGControls)
BW_QT_ADD_QRC(private ${PLUGIN_QT_COMMON_DIR}/resources/private)
BW_QT_ADD_CONFIG_QRC()
BW_GLOB_RESOURCE_FILES()

SET( ALL_SRCS
	plg_maya_adapter.cpp
    qt_application_adapter.hpp
    qt_application_adapter.cpp
    qt_framework_adapter.hpp
    qt_framework_adapter.cpp
    qt_window_adapter.hpp
    qt_window_adapter.cpp
    ${MAYA_APP_PLUGIN_DIR}/app/interfaces/i_application_adapter.hpp
    ${MAYA_APP_PLUGIN_DIR}/app/interfaces/i_application_listener.hpp
    ${MAYA_APP_PLUGIN_DIR}/app/interfaces/i_window_adapter.hpp
    ${MAYA_APP_PLUGIN_DIR}/app/interfaces/i_window_listener.hpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )


WG_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
	${BLOB_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

IF(BW_PLATFORM_WINDOWS)
	SET(PLATFORM_LIBRARIES shlwapi)
ELSE()
	SET(PLATFORM_LIBRARIES)
ENDIF()

IF (Qt5_WEB_ENGINE_SUPPORT)
	ADD_DEFINITIONS( -DUSE_Qt5_WEB_ENGINE=1 )
	SET(Qt5_WEB_ENGINE_LIBRARY Qt5::WebEngine)
ELSE()
	SET(Qt5_WEB_ENGINE_LIBRARY)
ENDIF()

BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	core_common
	core_generic_plugin
	core_qt_common
	core_ui_framework
	core_serialization_xml
    Qt5::Core
    Qt5::Gui
    Qt5::Quick
    Qt5::QuickWidgets
    Qt5::Widgets
	${Qt5_WEB_ENGINE_LIBRARY}
    
	# external libraries
	${PLATFORM_LIBRARIES}
)

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )