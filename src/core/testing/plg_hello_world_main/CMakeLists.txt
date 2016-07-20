CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )

# Name of plugin
PROJECT( plg_hello_world_main )
 
# This digs deeper into including the things necessary for your project to compile
INCLUDE( WGToolsCoreProject )

# Then we find QT5 which should have been set up in ngt/src/core/third_party/Qt as part of getting the source from Stash.
FIND_PACKAGE( QT5 )

# Add any files required for reflection
SET( METADATA_SRCS
    metadata/hello_panel.mpp
    metadata/hello_panel_list.mpp
)
SOURCE_GROUP( "MetaData" FILES ${METADATA_SRCS} )
 
# ALL_SRCS is a list of any uncategorized files that should be part of your project.
# These are all of the blank files you created earlier.
SET( ALL_SRCS
    hello_panel.hpp
    hello_panel.cpp
    hello_panel_exposed.hpp
    hello_panel_list.hpp
    hello_panel_list.cpp
    hello_panel_list_exposed.hpp
    plg_hello_world_main.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )
 
# First argument is name of qml module
# Second argument is path to all qml files
BW_QT_ADD_QRC( WGHello resources/WGHello )
BW_GLOB_RESOURCE_FILES()
 
BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
    ${ALL_SRCS}
    ${METADATA_SRCS}
    ${RESOURCE_FILES}
    ${COMPILED_RESOURCES}
)
 
# Libraries to link against
BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
    core_generic_plugin
    core_qt_common
    core_ui_framework
    Qt5::Core
)
 
# Grouping in the Visual Studio Solution Explorer
BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )