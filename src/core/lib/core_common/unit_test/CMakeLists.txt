CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_common_unit_test )

INCLUDE( WGToolsCoreProject )

SET( ALL_SRCS
	main.cpp
	test_wg_condition_variable.cpp
)

BW_BLOB_SOURCES( BLOB_SRCS ${ALL_SRCS} )
BW_ADD_EXECUTABLE( core_common_unit_test ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( core_common_unit_test
	core_common
	core_unit_test
)

BW_ADD_TOOL_TEST( core_common_unit_test )

BW_PROJECT_CATEGORY( core_common_unit_test "Unit Tests" )

