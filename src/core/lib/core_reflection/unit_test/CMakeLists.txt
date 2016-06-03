CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( reflection_unit_test )

INCLUDE( WGToolsCoreProject )


SET( ALL_SRCS
	main.cpp
	pch.hpp
	pch.cpp
	test_reflection_fixture.hpp
	test_reflection_fixture.cpp
	test_property_accessor.cpp
	test_class_definition.cpp
	test_methods.cpp
	test_function_property.cpp
	test_generic_object.cpp
	test_helpers.hpp
	test_objects.hpp
	test_objects.cpp
	test_reflected_property.cpp
	test_unmanaged_object.cpp
)

BW_BLOB_SOURCES( BLOB_SRCS ${ALL_SRCS} )
BW_ADD_EXECUTABLE( reflection_unit_test ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( reflection_unit_test PRIVATE
	core_reflection
	reflection_objects_test
	core_serialization
	core_unit_test
	core_reflection_utils
	core_command_system
	core_data_model
)

BW_ADD_TOOL_TEST( reflection_unit_test )

BW_PRECOMPILED_HEADER( reflection_unit_test pch.hpp )
BW_PROJECT_CATEGORY( reflection_unit_test "Unit Tests" )

