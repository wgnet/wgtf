CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

PROJECT( core_logging )

SET( ALL_SRCS
	logging.cpp
	logging.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )


BW_BLOB_SOURCES( BLOB_SRCS 
	${ALL_SRCS}
)
BW_ADD_LIBRARY( core_logging ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( core_logging
INTERFACE
core_common
)


BW_PROJECT_CATEGORY( core_logging "WGT Libs" )

