CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_dependency_system )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

SET( ALL_SRCS
	i_interface.hpp
	interface_helpers.hpp
	di_ref.hpp
	di_ref.cpp
	depends.hpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS 
	${ALL_SRCS} 
)
BW_ADD_LIBRARY( core_dependency_system ${BLOB_SRCS} )

BW_PROJECT_CATEGORY( core_dependency_system "WGT Libs" )

