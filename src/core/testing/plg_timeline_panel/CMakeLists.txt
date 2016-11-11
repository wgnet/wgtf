CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
# Name of plugin
PROJECT( plg_timeline_panel )
 
# This digs deeper into including the things necessary for your project to compile
INCLUDE( WGToolsCoreProject )
# Then we find QT5 which should have been set up in ngt/src/core/third_party/Qt as part of getting the source from Stash.
FIND_PACKAGE( QT5 )
 
# ALL_SRCS is a list of any uncategorized files that should be part of your project.
# These are all of the blank files you created earlier.
SET( ALL_SRCS
	timeline_model.hpp
	timeline_model.cpp
    timeline_panel.hpp
    timeline_panel.cpp
    plg_timeline_panel_main.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )
 
BW_QT_ADD_QRC( PlgTimelinePanel resources/PlgTimelinePanel )
BW_GLOB_RESOURCE_FILES()
 
BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
    ${ALL_SRCS}
    ${RESOURCE_FILES}
    ${COMPILED_RESOURCES}
)
 
# Libraries to link against
BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
    core_generic_plugin
	core_data_model
)
 
# Grouping in the Visual Studio Solution Explorer
BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )
