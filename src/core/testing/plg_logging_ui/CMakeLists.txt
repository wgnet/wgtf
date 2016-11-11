CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )

PROJECT( plg_logging_ui )

INCLUDE( WGToolsCoreProject )

FIND_PACKAGE( QT5 )

SET( ALL_SRCS
	metadata/logging_view_model.mpp
    logging_data_model.hpp
    logging_data_model.cpp
    logging_view_model.hpp
    logging_view_model.cpp
    plg_logging_ui.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )
 
BW_QT_ADD_QRC( ${PROJECT_NAME} resources/plg_logging_ui )
BW_GLOB_RESOURCE_FILES()
 
BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
    ${ALL_SRCS}
    ${RESOURCE_FILES}
    ${COMPILED_RESOURCES}
)
 
# Libraries to link against
BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
    core_generic_plugin
    core_logging_system
)
 
# Grouping in the Visual Studio Solution Explorer
BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )
