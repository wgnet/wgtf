CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_object )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( ALL_SRCS
	interfaces/i_object_manager.hpp
	context_object_manager.cpp
	context_object_manager.hpp
	object_manager.hpp
	object_manager_creator.cpp
	object_manager_creator.hpp
	plg_object.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_object SHARED
	${ALL_SRCS}
)

# Must be copied to unit test bin
BW_COPY_PLUGIN_TO_UNIT_TEST_BIN( plg_object )

BW_TARGET_LINK_LIBRARIES( plg_object PRIVATE
	core_generic_plugin
)

BW_PROJECT_CATEGORY( plg_object "Plugins" )
