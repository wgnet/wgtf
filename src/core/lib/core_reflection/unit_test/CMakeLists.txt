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
	test_meta_animated.cpp
	test_meta_animated_float_object.cpp
	test_meta_animated_float_object.hpp
	test_meta_animated_object.cpp
	test_meta_animated_object.hpp
	test_meta_animated_vector_object.cpp
	test_meta_animated_vector_object.hpp
	test_methods.cpp
	test_function_property.cpp
	test_generic_object.cpp
	test_helpers.hpp
	test_objects.hpp
	test_objects.cpp
	test_reflected_property.cpp
	test_object_handle.cpp
	test_object_handle_fixture.hpp
	test_managed_object.cpp
	test_meta_data.cpp
)

WG_BLOB_SOURCES( BLOB_SRCS ${ALL_SRCS} )
BW_ADD_EXECUTABLE(  ${PROJECT_NAME} ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES(  ${PROJECT_NAME} PRIVATE
	reflection_objects_test
	core_unit_test
	core_command_system
	core_data_model
    core_environment_system
	core_serialization_xml
)

BW_ADD_TOOL_TEST(  ${PROJECT_NAME} )

WG_PRECOMPILED_HEADER(  ${PROJECT_NAME} pch.hpp )
BW_PROJECT_CATEGORY(  ${PROJECT_NAME} "Unit Tests" )

