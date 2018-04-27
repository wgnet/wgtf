CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_logging )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

ADD_DEFINITIONS( -DWG_LOGGING_EXPORT )

SET( ALL_SRCS
	logging.cpp
	logging.hpp
	log_level.hpp
	wg_logging_dll.hpp
)

WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

WG_BLOB_SOURCES( BLOB_SRCS 
	${ALL_SRCS}
)
BW_ADD_LIBRARY( core_logging SHARED ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( core_logging
INTERFACE
core_common
)


BW_PROJECT_CATEGORY( core_logging "WGT Libs" )