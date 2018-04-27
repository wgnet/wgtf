CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_idedebug_logger )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS( 
	-D_CRT_SECURE_NO_WARNINGS 
)

SET( ALL_SRCS
	plg_idedebug_logger.cpp
	windows_debug_logger.cpp
	windows_debug_logger.hpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

WG_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
 
BW_ADD_TOOL_PLUGIN( plg_idedebug_logger SHARED
	${BLOB_SRCS}
)

BW_TARGET_LINK_LIBRARIES( plg_idedebug_logger PRIVATE
	core_generic_plugin
	core_logging_system
)

BW_PROJECT_CATEGORY( plg_idedebug_logger "Plugins" )


