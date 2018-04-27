CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_reflection )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)


SET( ALL_SRCS
	context_definition_manager.cpp
	context_definition_manager.hpp
	context_object_manager.cpp
	context_object_manager.hpp	
	plg_reflection.cpp
	reflection_system_holder.hpp
	reflection_system_holder.cpp
	reflection_component_provider.hpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_reflection SHARED
	${ALL_SRCS}
)

# Must be copied to unit test bin
BW_COPY_PLUGIN_TO_UNIT_TEST_BIN( plg_reflection )

BW_TARGET_LINK_LIBRARIES( plg_reflection PRIVATE
	core_generic_plugin
	core_reflection
	core_reflection_utils
	core_object
)

BW_PROJECT_CATEGORY( plg_reflection "Plugins" )
