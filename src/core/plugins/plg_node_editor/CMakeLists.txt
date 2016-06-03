cmake_minimum_required (VERSION 3.0)
project( plg_node_editor )

#find_package(NGT REQUIRED)

INCLUDE( WGToolsProject )
INCLUDE( WGQtLibrary )

BW_QT_ADD_QRC(plg_node_editor resources)
BW_GLOB_RESOURCE_FILES()

#LIST(APPEND TARGET_LIBRARIES core_generic_plugin core_qt_common core_ui_framework)

#include_directories   ( ${CMAKE_CURRENT_BINARY_DIR})
#include_directories   ( "src" )
#include_directories   (${ADDITIONAL_INCLUDES})

add_definitions       (-DNGT_INTEGRATION)

FILE(GLOB_RECURSE QML_SRCS *.qml *.js *.ui *.xml)
SET(QT_RESOURCE_FILES
  ${QML_SRCS}
)

SOURCE_GROUP("Resource Files" FILES ${RESOURCE_FILES} )
SOURCE_GROUP("Compiled Resources" FILES ${COMPILED_RESOURCES})

FILE(GLOB_RECURSE METADATA_SRCS "metadata/*.mpp")
SOURCE_GROUP("Metadata" FILES ${METADATA_SRCS})

SET(INTERFACE_SRCS
	interfaces/i_graph.hpp
	interfaces/i_node.hpp
	interfaces/i_node_editor.hpp
    interfaces/i_slot.hpp
    interfaces/i_connection.hpp
)
SOURCE_GROUP("Interfaces" FILES ${INTERFACE_SRCS})

SET( ALL_SRCS
    src/connection_curve.hpp
    src/connection_curve.cpp
	src/node_editor.hpp
	src/node_editor.cpp
	plg_node_editor.hpp    
	plg_node_editor.cpp
)                                     
SOURCE_GROUP( "Source Files" FILES ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_node_editor SHARED
	${ALL_SRCS}
	${METADATA_SRCS}
	${QT_RESOURCE_FILES}
	${COMPILED_RESOURCES}
	${RESOURCE_FILES}
	${INTERFACE_SRCS}
)

BW_TARGET_LINK_LIBRARIES( plg_node_editor PRIVATE
	core_generic_plugin
	core_qt_common
	core_qt_script
	core_ui_framework
	Qt5::Core
    Qt5::Gui
    Qt5::QuickWidgets
    Qt5::Quick

)

BW_PROJECT_CATEGORY( plg_node_editor "Plugins" )