CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )

PROJECT( plg_logging_ui )

INCLUDE( WGToolsCoreProject )

FIND_PACKAGE( QT5 )

SET( METADATA_SRCS
	metadata/logging_view_model.mpp
)
SOURCE_GROUP( "MetaData" FILES ${METADATA_SRCS} )

SET( ALL_SRCS
    logging_data_model.hpp
    logging_data_model.cpp
    logging_view_model.hpp
    logging_view_model.cpp
    plg_logging_ui.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )
 
BW_QT_ADD_QRC( ${PROJECT_NAME} resources/plg_logging_ui )
BW_GLOB_RESOURCE_FILES()
 
BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
    ${ALL_SRCS}
    ${RESOURCE_FILES}
    ${COMPILED_RESOURCES}
    ${METADATA_SRCS}
)
 
# Libraries to link against
BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
    core_generic_plugin
    core_qt_common
    core_ui_framework
    core_logging_system
    Qt5::Core
    Qt5::Gui
    Qt5::Quick
    Qt5::Widgets
)
 
# Grouping in the Visual Studio Solution Explorer
BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )
