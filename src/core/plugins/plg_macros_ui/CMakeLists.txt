CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_macros_ui )

INCLUDE( WGToolsCoreProject )
find_package(QT5 ${QT_FIND_PACKAGE_VERSION})

SET( ALL_SRCS
	metadata/macros_object.mpp
	metadata/macro_display.mpp
	macro_display.cpp
	macro_display.hpp
	macros_object.cpp
	macros_object.hpp
	macros_model.cpp
	macros_model.hpp
	plg_macros_ui.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_QT_ADD_QRC(WGMacros resources/WGMacros)
BW_GLOB_RESOURCE_FILES()

WG_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_TOOL_PLUGIN( plg_macros_ui SHARED
	${BLOB_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)


BW_TARGET_LINK_LIBRARIES( plg_macros_ui PRIVATE
	core_command_system
	core_reflection
	core_object
	core_generic_plugin
	core_data_model
	core_serialization_xml
)

BW_PROJECT_CATEGORY( plg_macros_ui "Plugins" )

