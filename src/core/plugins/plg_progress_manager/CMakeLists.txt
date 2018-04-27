CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_progress_manager )

INCLUDE( WGToolsCoreProject )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS(
	-D_CRT_SECURE_NO_WARNINGS
)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( ALL_SRCS
	plg_progress_manager.cpp
	progress_manager.hpp
	progress_manager.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )


BW_ADD_TOOL_PLUGIN( plg_progress_manager SHARED
	${ALL_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( plg_progress_manager PRIVATE
	core_generic_plugin
	core_command_system
	core_serialization_xml
)

BW_PROJECT_CATEGORY( plg_progress_manager "Plugins" )
