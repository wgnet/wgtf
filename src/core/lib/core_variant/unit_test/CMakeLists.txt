CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( variant_unit_test )

INCLUDE( WGToolsCoreProject )

SET( ALL_SRCS
	main.cpp
	pch.cpp
	pch.hpp
	test_collection.cpp
	test_variant.cpp
)

BW_BLOB_SOURCES( BLOB_SRCS ${ALL_SRCS} )
BW_ADD_EXECUTABLE( variant_unit_test ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( variant_unit_test PRIVATE
	core_variant
	core_unit_test
	)

BW_ADD_TOOL_TEST( variant_unit_test )

BW_PRECOMPILED_HEADER( variant_unit_test pch.hpp )
BW_PROJECT_CATEGORY( variant_unit_test "Unit Tests" )
