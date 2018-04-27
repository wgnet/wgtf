CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_object )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

SET( ALL_SRCS
	i_object_manager.hpp
	i_object_handle_storage.hpp
	i_object_handle_provider.hpp
	i_managed_object.hpp
	managed_object.hpp
	object_reference.hpp
	object_manager.hpp
	object_manager.cpp
	object_handle_storage.hpp
	object_handle_storage.cpp
	object_handle_storage_shared.hpp
	object_handle_variant_storage.hpp
	object_handle_stream_utils.hpp
	object_handle_cast_utils.hpp
	object_handle_provider.hpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

WG_BLOB_SOURCES( BLOB_SRCS 
	${ALL_SRCS} 
)
BW_ADD_LIBRARY( core_object ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( core_object PRIVATE
	wgtf_types
)

BW_PROJECT_CATEGORY( core_object "WGT Libs" )

