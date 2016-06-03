CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_serialization )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( ALL_SRCS
	plg_serialization.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_serialization SHARED
	${ALL_SRCS}
)

BW_TARGET_LINK_LIBRARIES( plg_serialization PRIVATE
	core_generic_plugin
	core_serialization )

BW_PROJECT_CATEGORY( plg_serialization "Plugins" )
