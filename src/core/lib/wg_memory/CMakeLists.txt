CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( wg_memory )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

ADD_DEFINITIONS( -DWG_MEMORY_EXPORT )

SET( ALL_SRCS
	allocator.hpp
	allocator.cpp
	memory_overrides.hpp
	wg_memory_dll.hpp
)
SOURCE_GROUP( "Source" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS} 
)
BW_ADD_LIBRARY( wg_memory SHARED ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( wg_memory
	core_common
)

BW_PROJECT_CATEGORY( wg_memory "Wargaming Libs" )

