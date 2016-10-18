CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_command_system )

INCLUDE( WGToolsCoreProject )

REMOVE_DEFINITIONS( -WX )
ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)


SET( ALL_SRCS
	plg_command_system.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_command_system SHARED
	${ALL_SRCS}
)

BW_TARGET_LINK_LIBRARIES( plg_command_system PRIVATE
	core_reflection
	core_generic_plugin
	core_command_system
)

BW_PROJECT_CATEGORY( plg_command_system "Plugins" )
