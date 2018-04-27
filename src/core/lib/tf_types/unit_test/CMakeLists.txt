CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( tf_types_unit_test )

INCLUDE( WGToolsCoreProject )

SET( ALL_SRCS
	main.cpp
	pch.hpp
	pch.cpp
	test_color_utilities.cpp
)

WG_BLOB_SOURCES( BLOB_SRCS ${ALL_SRCS} )
BW_ADD_EXECUTABLE(  ${PROJECT_NAME} ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES(  ${PROJECT_NAME} PRIVATE
	tf_types
	core_unit_test
)

BW_ADD_TOOL_TEST(  ${PROJECT_NAME} )

WG_PRECOMPILED_HEADER(  ${PROJECT_NAME} pch.hpp )
BW_PROJECT_CATEGORY(  ${PROJECT_NAME} "Unit Tests" )

