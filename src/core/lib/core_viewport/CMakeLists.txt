CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_viewport )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

SET( ALL_SRCS
	viewport.hpp
	viewport.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

WG_BLOB_SOURCES( BLOB_SRCS 
	${ALL_SRCS} 
)
BW_ADD_LIBRARY( core_viewport ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( core_viewport INTERFACE
	core_ui_framework
	core_environment_system
)

BW_PROJECT_CATEGORY( core_viewport "WGT Libs" )

