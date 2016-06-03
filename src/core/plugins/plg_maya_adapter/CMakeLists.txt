CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_maya_adapter )

INCLUDE( WGToolsCoreProject )
find_package(QT5)

SET( PLUGIN_QT_APP_DIR ${CMAKE_CURRENT_LIST_DIR}/../plg_qt_app )
SET( PLUGIN_QT_COMMON_DIR ${CMAKE_CURRENT_LIST_DIR}/../plg_qt_common )
SET( MAYA_APP_PLUGIN_DIR ${CMAKE_CURRENT_LIST_DIR}/../../app/maya_plugin )

INCLUDE_DIRECTORIES( ${PLUGIN_QT_APP_DIR} )
INCLUDE_DIRECTORIES( ${PLUGIN_QT_COMMON_DIR} )
INCLUDE_DIRECTORIES( ${MAYA_APP_PLUGIN_DIR}/app/interfaces )

BW_QT_ADD_QRC(WGControls ${PLUGIN_QT_COMMON_DIR}/resources/WGControls)
BW_GLOB_RESOURCE_FILES()

SET( ALL_SRCS
	plg_maya_adapter.cpp
    qt_application_adapter.hpp
    qt_application_adapter.cpp
    qt_framework_adapter.hpp
    qt_framework_adapter.cpp
    qt_window_adapter.hpp
    qt_window_adapter.cpp
	${PLUGIN_QT_COMMON_DIR}/qt_framework.cpp
	${PLUGIN_QT_COMMON_DIR}/qt_framework.hpp
	${PLUGIN_QT_COMMON_DIR}/qt_preferences.cpp
	${PLUGIN_QT_COMMON_DIR}/qt_preferences.hpp
	${PLUGIN_QT_COMMON_DIR}/qt_copy_paste_manager.cpp
	${PLUGIN_QT_COMMON_DIR}/qt_copy_paste_manager.hpp
	${PLUGIN_QT_COMMON_DIR}/private/ui_view_creator.hpp
	${PLUGIN_QT_COMMON_DIR}/private/ui_view_creator.cpp
	${PLUGIN_QT_APP_DIR}/qt_application.cpp
	${PLUGIN_QT_APP_DIR}/qt_application.hpp
    ${MAYA_APP_PLUGIN_DIR}/app/interfaces/i_application_adapter.hpp
    ${MAYA_APP_PLUGIN_DIR}/app/interfaces/i_application_listener.hpp
    ${MAYA_APP_PLUGIN_DIR}/app/interfaces/i_window_adapter.hpp
    ${MAYA_APP_PLUGIN_DIR}/app/interfaces/i_window_listener.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )


BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
	${BLOB_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	core_common
	core_generic_plugin
	core_qt_common
	core_qt_script
	core_ui_framework
    Qt5::Core
    Qt5::Gui
    Qt5::Quick
    Qt5::QuickWidgets
    Qt5::Widgets
)

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )

SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD_DEBUG True)