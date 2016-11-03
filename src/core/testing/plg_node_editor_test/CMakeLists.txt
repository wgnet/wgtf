cmake_minimum_required (VERSION 3.0)
project( plg_node_editor_test )

#find_package(NGT REQUIRED)

INCLUDE( WGToolsProject )

#LIST(APPEND TARGET_LIBRARIES core_generic_plugin core_qt_common core_ui_framework)

#include_directories   ( ${CMAKE_CURRENT_BINARY_DIR})
#include_directories   (${ADDITIONAL_INCLUDES})

SOURCE_GROUP("Resource Files" FILES ${RESOURCE_FILES} )
SOURCE_GROUP("Compiled Resources" FILES ${COMPILED_RESOURCES})

SET( ALL_SRCS
	implements/CustomGraph.h
	implements/CustomGraph.cpp
	implements/CustomConnection.h
	implements/CustomConnection.cpp
    implements/AddIntegerNode.h
    implements/AddIntegerNode.cpp
    implements/EventSlot.h
    implements/EventSlot.cpp
    implements/IntegerSlot.h
    implements/IntegerSlot.cpp
    implements/IntToStringNode.h
    implements/IntToStringNode.cpp
    implements/PrintNode.h
    implements/PrintNode.cpp
    implements/StringSlot.h
    implements/StringSlot.cpp
	plg_node_editor_test.cpp
)                                     
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_node_editor_test SHARED
	${ALL_SRCS}
)

BW_TARGET_LINK_LIBRARIES( plg_node_editor_test PRIVATE
	core_generic_plugin
	core_qt_common
	core_qt_script
	core_ui_framework
)

BW_PROJECT_CATEGORY( plg_node_editor_test "Plugins" )

