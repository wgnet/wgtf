CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_dependency_system )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

SET( ALL_SRCS
	context_callback_helper.hpp
	i_interface.hpp
	interface_helpers.hpp
	depends.hpp
	depends.cpp
	wgt_static_init.hpp
	wgt_static_init.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

WG_BLOB_SOURCES( BLOB_SRCS 
	${ALL_SRCS} 
)
BW_ADD_LIBRARY( core_dependency_system ${BLOB_SRCS} )

BW_PROJECT_CATEGORY( core_dependency_system "WGT Libs" )

