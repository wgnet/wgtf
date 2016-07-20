CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_file_system )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS( 
	-D_CRT_SECURE_NO_WARNINGS
)

SET( ALL_SRCS
	plg_file_system.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
 
BW_ADD_TOOL_PLUGIN( plg_file_system SHARED
	${BLOB_SRCS}
)

BW_TARGET_LINK_LIBRARIES( plg_file_system PRIVATE
	core_generic_plugin
	core_reflection
	core_string_utils
)

BW_PROJECT_CATEGORY( plg_file_system "Plugins" )


