CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_string_utils )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

SET( ALL_SRCS
	file_path.hpp
	string_utils.hpp
	string_utils.cpp
	text_utilities.hpp
	text_utilities.cpp
	time_utils.hpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

WG_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS} 
)
BW_ADD_LIBRARY( core_string_utils ${BLOB_SRCS} )

BW_PROJECT_CATEGORY( core_string_utils "WGT Libs" )

