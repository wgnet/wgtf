CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_generic_plugin_manager )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

SET( ALL_SRCS
	config_plugin_loader.hpp
	config_plugin_loader.cpp
	default_context_manager.hpp
	default_context_manager.cpp
	folder_plugin_loader.hpp
	folder_plugin_loader.cpp
	generic_plugin_manager.hpp
	generic_plugin_manager.cpp
	plugin_context_manager.hpp
	plugin_context_manager.cpp
	plugin_static_initializer.hpp
	plugin_static_initializer.cpp
	notify_plugin.hpp
	notify_plugin.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )


WG_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_LIBRARY( core_generic_plugin_manager ${BLOB_SRCS} )

IF(BW_PLATFORM_WINDOWS)
	SET(PLATFORM_LIBRARIES
		shlwapi)
ELSE()
	SET(PLATFORM_LIBRARIES)
ENDIF()

BW_TARGET_LINK_LIBRARIES( core_generic_plugin_manager INTERFACE
	core_common
	core_reflection
	core_dependency_system
	core_logging

	# external libraries
	${PLATFORM_LIBRARIES}
)

BW_PROJECT_CATEGORY( core_generic_plugin_manager "WGT Libs/Plugin System" )
