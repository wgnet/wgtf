cmake_minimum_required (VERSION 3.0)
project( plg_node_editor )

INCLUDE( WGToolsProject )
INCLUDE( WGQtLibrary )

SET(ALL_SRCS
	interfaces/i_graph.hpp
	interfaces/i_group.hpp
	interfaces/i_node.hpp
	interfaces/i_node_editor.hpp
	interfaces/i_slot.hpp
	interfaces/i_connection.hpp
	src/connection_curve.hpp
	src/connection_curve.cpp
	src/group.hpp
	src/group.cpp
	src/node_editor.hpp
	src/node_editor.cpp
	plg_node_editor.hpp    
	plg_node_editor.cpp
    metadata/i_connection.mpp
    metadata/i_graph.mpp
    metadata/i_node.mpp
    metadata/i_group.mpp
    metadata/i_node_editor.mpp
    metadata/i_slot.mpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_QT_ADD_QRC(plg_node_editor resources/plg_node_editor)
BW_GLOB_RESOURCE_FILES()

BW_ADD_TOOL_PLUGIN( plg_node_editor SHARED
	${ALL_SRCS}
	${COMPILED_RESOURCES}
	${RESOURCE_FILES}
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
