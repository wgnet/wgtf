CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_environment_system )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

SET( ALL_SRCS
	i_env_system.hpp
	env_system.cpp
	env_system.hpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

WG_BLOB_SOURCES( BLOB_SRCS 
	${ALL_SRCS} 
)
BW_ADD_LIBRARY( core_environment_system ${BLOB_SRCS} )

BW_PROJECT_CATEGORY( core_environment_system "WGT Libs" )

