CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_variant )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

ADD_DEFINITIONS( -DVARIANT_EXPORT )

SET( ALL_SRCS
	stl_container/linear_collection.hpp
	stl_container/map_collection.hpp
	collection.cpp
	collection.hpp
	container_helper.hpp
	container_helper.cpp
	meta_type.cpp
	meta_type.hpp
	standard_metatypes.cpp
	standard_metatypes.hpp
	type_id.hpp
	type_id.cpp
	variant.cpp
	variant.hpp
	variant_dll.hpp
	wg_types_metatypes.hpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

WG_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_LIBRARY( core_variant ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( core_variant INTERFACE
	wgtf_types
)

BW_PROJECT_CATEGORY( core_variant "WGT Libs" )

