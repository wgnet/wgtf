CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_qt_common )

INCLUDE( WGToolsCoreProject )
find_package(QT5)

SET( ALL_SRCS
    plg_qt_common.cpp
    qt_copy_paste_manager.cpp
    qt_copy_paste_manager.hpp
	qt_framework.cpp
	qt_framework.hpp
	qt_preferences.cpp
	qt_preferences.hpp
    qt_action_manager.cpp
	qt_action_manager.hpp
	private/ui_view_creator.hpp
	private/ui_view_creator.cpp
	private/qt_ui_worker.hpp
	private/qt_ui_worker.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_QT_ADD_QRC(WGControls resources/WGControls)
BW_QT_ADD_QRC(private resources/private)
BW_GLOB_RESOURCE_FILES()

FILE( GLOB FONT_FILES resources/fonts/* )
SOURCE_GROUP( "Font Files" FILES ${FONT_FILES} )
BW_CUSTOM_COPY_TO_PROJECT_OUTPUT( .. "${FONT_FILES}" )

BW_BLOB_SOURCES( BLOB_SRCS
    ${ALL_SRCS}
)

BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
	${BLOB_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
	${FONT_FILES}
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
)

BW_DEPLOY_QT()

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )
