CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( gps_unit_test )

INCLUDE( WGToolsCoreProject )

SET( ALL_SRCS
	main.cpp
	pch.hpp
	pch.cpp
	test_plugin_system.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )


BW_BLOB_SOURCES( BLOB_SRCS ${ALL_SRCS} )
BW_ADD_EXECUTABLE( gps_unit_test ${BLOB_SRCS} )
BW_TARGET_LINK_LIBRARIES( gps_unit_test PRIVATE
	core_generic_plugin_manager
	core_generic_plugin_test
	core_reflection
	core_unit_test
)

BW_ADD_TOOL_TEST( gps_unit_test )

BW_PRECOMPILED_HEADER( gps_unit_test pch.hpp )
BW_PROJECT_CATEGORY( gps_unit_test "Unit Tests" )

ADD_DEPENDENCIES( gps_unit_test plg_reflection )
ADD_DEPENDENCIES( gps_unit_test plg_plugin1_test )
ADD_DEPENDENCIES( gps_unit_test plg_plugin2_test )

