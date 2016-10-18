CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_generic_plugin )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

SET( ALL_SRCS 
	interfaces/i_application.hpp
	interfaces/i_component_context.hpp
	interfaces/i_memory_allocator.hpp
	interfaces/i_component_context_creator.hpp
	interfaces/i_plugin_context_manager.hpp
	interfaces/ui_connection.hpp
	generic_plugin.hpp
	generic_plugin.cpp
)

WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_LIBRARY( core_generic_plugin ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( core_generic_plugin INTERFACE
	core_common
	core_reflection
	core_dependency_system
	core_logging
)

BW_PROJECT_CATEGORY( core_generic_plugin "WGT Libs/Plugin System" )

