CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_logging_system )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS( 
	-D_CRT_SECURE_NO_WARNINGS 
)

SET( ALL_SRCS
	plg_logging_system.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
 
BW_ADD_TOOL_PLUGIN( plg_logging_system SHARED
	${BLOB_SRCS}
)

BW_TARGET_LINK_LIBRARIES( plg_logging_system PRIVATE
	core_generic_plugin
	core_logging_system
)

BW_PROJECT_CATEGORY( plg_logging_system "Plugins" )


