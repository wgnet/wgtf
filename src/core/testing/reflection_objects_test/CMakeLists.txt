CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( reflection_objects_test )

IF( MSVC )
	SET( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj" )
ENDIF()

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )


SET( ALL_SRCS
	metadata/test_structure.mpp
	metadata/test_polystruct.mpp
	metadata/test_methods_object.mpp
	test_macros.hpp
	test_objects.hpp
	test_objects.cpp
	test_polystruct.hpp
	test_polystruct.cpp
	test_structure.hpp
	test_structure.cpp
	test_methods_object.hpp
	test_methods_object.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )


WG_BLOB_SOURCES( BLOB_SRCS
	${METADATA_SRCS}
	${ALL_SRCS}
)
BW_ADD_LIBRARY( reflection_objects_test ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( reflection_objects_test INTERFACE )

BW_PROJECT_CATEGORY( reflection_objects_test "Test Data" )
