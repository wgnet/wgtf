CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
# Name of plugin
PROJECT( plg_hello_world_interface )
 
# This digs deeper into including the things necessary for your project to compile
INCLUDE( WGToolsCoreProject )

# Then we find QT5 which should have been set up in ngt/src/core/third_party/Qt as part of getting the source from Stash.
FIND_PACKAGE( QT5 ${QT_FIND_PACKAGE_VERSION} )
 
# ALL_SRCS is a list of any uncategorized files that should be part of your project.
# These are all of the blank files you created earlier.
SET( ALL_SRCS
    hello_interface_impl.hpp
    hello_interface_impl.cpp
    plg_hello_world_interface.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )
 
# First argument is name of qml module
# Second argument is path to all qml files
BW_QT_ADD_QRC( WGHelloInterface resources/WGHelloInterface )
BW_GLOB_RESOURCE_FILES()
 
BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
    ${ALL_SRCS}
    ${RESOURCE_FILES}
    ${COMPILED_RESOURCES}
)
 
# Libraries to link against
BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
    core_generic_plugin
)
 
# Grouping in the Visual Studio Solution Explorer
BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )
