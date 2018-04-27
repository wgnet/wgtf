CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )
PROJECT( core_view_model )

SET( METADATA
	metadata/property_view_model.mpp
	metadata/view_model.mpp
)
WG_AUTO_SOURCE_GROUPS(${METADATA})

SET( ALL_SRCS
	property_view_model.hpp
	view_model.cpp
	view_model.hpp
)
WG_AUTO_SOURCE_GROUPS(${ALL_SRCS})

WG_BLOB_SOURCES( BLOB_SRCS 
	${METADATA}
	${ALL_SRCS}
)

BW_ADD_LIBRARY( core_view_model ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( core_view_model INTERFACE
	core_reflection
	core_reflection_utils
)

BW_PROJECT_CATEGORY( core_view_model "WGT Libs/View Model" )

