CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_3_test )

INCLUDE( WGToolsCoreProject )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( ALL_SRCS
	plg_3_test.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_3_test SHARED
	${ALL_SRCS}
)

BW_TARGET_LINK_LIBRARIES( plg_3_test PRIVATE
	core_generic_plugin )

BW_PROJECT_CATEGORY( plg_3_test "Plugins" )
