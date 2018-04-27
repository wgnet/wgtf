CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_string_utils_unit_test )

INCLUDE( WGToolsCoreProject )

SET( ALL_SRCS
	main.cpp
	pch.cpp
	pch.hpp
	test_string_utils.cpp
)

WG_BLOB_SOURCES( BLOB_SRCS ${ALL_SRCS} )
BW_ADD_EXECUTABLE( core_string_utils_unit_test ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( core_string_utils_unit_test PRIVATE
	core_string_utils
	core_unit_test
)

BW_ADD_TOOL_TEST( core_string_utils_unit_test )

WG_PRECOMPILED_HEADER( core_string_utils_unit_test pch.hpp )
BW_PROJECT_CATEGORY( core_string_utils_unit_test "Unit Tests" )