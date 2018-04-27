CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_command_system_unit_test )

INCLUDE( WGToolsCoreProject )

SET( ALL_SRCS
	main.cpp
	pch.cpp
	pch.hpp
	test_helpers.hpp
	test_command_system_fixture.hpp
	test_command_system_fixture.cpp
	test_command_system.cpp
	test_objects.hpp
	test_objects.cpp
	reflection_auto_reg.mpp
)

WG_BLOB_SOURCES( BLOB_SRCS ${ALL_SRCS} )
BW_ADD_EXECUTABLE( core_command_system_unit_test ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( core_command_system_unit_test PRIVATE
	core_command_system
	core_unit_test
    core_environment_system
	core_serialization_xml
)

BW_ADD_TOOL_TEST( core_command_system_unit_test )

WG_PRECOMPILED_HEADER( core_command_system_unit_test pch.hpp )
BW_PROJECT_CATEGORY( core_command_system_unit_test "Unit Tests" )

