CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( wgtf_types  )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

SET( ALL_SRCS
	binary_block.hpp
	binary_block.cpp
	color_utilities.hpp
	color_utilities.cpp
	hash_utilities.hpp
	hash_utilities.cpp
	hashed_string_ref.hpp
	hashed_string_ref.cpp
	shared_string.hpp
	shared_string.cpp
	string_ref.hpp
	string_ref.cpp
    vector2.hpp
    vector3.hpp
    vector4.hpp
	base64.cpp
	base64.hpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

WG_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS} 
)
BW_ADD_LIBRARY( ${PROJECT_NAME} ${BLOB_SRCS} )

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Wargaming Libs" )

