CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_variant )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

SET( INTERFACES_SRC
	interfaces/i_meta_type_manager.hpp
)
SOURCE_GROUP( "interfaces" FILES ${INTERFACES_SRC} )

SET( STL_CONTAINER_SRC
	stl_container/linear_collection.hpp
	stl_container/map_collection.hpp
)
SOURCE_GROUP( "stl_container" FILES ${STL_CONTAINER_SRC} )

SET( ALL_SRCS
	collection.cpp
	collection.hpp
	container_helper.hpp
	container_helper.cpp
	default_meta_type_manager.hpp
	default_meta_type_manager.cpp
	meta_type.cpp
	meta_type.hpp
	type_id.hpp
	type_id.cpp
	variant.cpp
	variant.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${INTERFACES_SRC}
	${STL_CONTAINER_SRC}
	${ALL_SRCS}
)
BW_ADD_LIBRARY( core_variant ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( core_variant PRIVATE
	wg_types
	core_serialization
)

BW_PROJECT_CATEGORY( core_variant "WGT Libs" )

