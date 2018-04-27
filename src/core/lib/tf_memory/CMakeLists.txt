CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( tf_memory )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

ADD_DEFINITIONS( -DWG_MEMORY_EXPORT )

SET( ALL_SRCS
	allocator.hpp
	allocator.cpp
	memory_overrides.hpp
	wg_memory_dll.hpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

WG_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS} 
)
BW_ADD_LIBRARY( ${PROJECT_NAME} SHARED ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME}
	core_common
	tf_types
)

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Wargaming Libs" )

