CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_editor_interaction )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)


SET( ALL_SRCS
	plg_editor_interaction.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_editor_interaction SHARED
	${ALL_SRCS}
)

BW_TARGET_LINK_LIBRARIES( plg_editor_interaction PRIVATE
	core_data_model_cmds
	core_generic_plugin
	core_reflection
	core_reflection_utils
	core_command_system )

BW_PROJECT_CATEGORY( plg_editor_interaction "Plugins" )
