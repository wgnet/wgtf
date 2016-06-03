CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_variant )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( ALL_SRCS
	plg_variant.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_variant SHARED
	${ALL_SRCS}
)

# Must be copied to unit test bin
BW_COPY_PLUGIN_TO_UNIT_TEST_BIN( plg_variant )

BW_TARGET_LINK_LIBRARIES( plg_variant PRIVATE
	core_generic_plugin
	core_variant )

BW_PROJECT_CATEGORY( plg_variant "Plugins" )
