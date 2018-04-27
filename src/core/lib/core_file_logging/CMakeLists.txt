CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_file_logging )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

SET( ALL_SRCS
	file_logger.hpp
	file_logger.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

WG_BLOB_SOURCES( BLOB_SRCS 
	${ALL_SRCS} 
)
BW_ADD_LIBRARY( core_file_logging ${BLOB_SRCS} )

IF(BW_PLATFORM_WINDOWS)
	SET(PLATFORM_LIBRARIES psapi)
ELSE()
	SET(PLATFORM_LIBRARIES)
ENDIF()

BW_TARGET_LINK_LIBRARIES( core_file_logging INTERFACE
	wgtf_memory
	
	# external libraries
	${PLATFORM_LIBRARIES}  	
)

BW_PROJECT_CATEGORY( core_file_logging "WGT Libs" )

