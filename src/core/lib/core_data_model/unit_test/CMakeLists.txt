CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_data_model_unit_test )

INCLUDE( WGToolsCoreProject )

SET( ALL_SRCS
	main.cpp
	pch.cpp
	pch.hpp
	test_data_model_objects.hpp
	test_data_model_objects.cpp
	test_data_model.cpp
	test_data_model_fixture.hpp
	test_data_model_fixture.cpp
	test_string_data.hpp
	test_string_data.cpp
)

BW_BLOB_SOURCES( BLOB_SRCS ${ALL_SRCS} )
BW_ADD_EXECUTABLE( core_data_model_unit_test ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( core_data_model_unit_test PRIVATE
	core_data_model
	core_unit_test
)

BW_ADD_TOOL_TEST( core_data_model_unit_test )

BW_PRECOMPILED_HEADER( core_data_model_unit_test pch.hpp )
BW_PROJECT_CATEGORY( core_data_model_unit_test "Unit Tests" )

