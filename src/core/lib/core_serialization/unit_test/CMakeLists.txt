CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( serialization_unit_test )

INCLUDE( WGToolsCoreProject )

SET( ALL_SRCS
	main.cpp
	pch.cpp
	pch.hpp
	test_datastreambuf.cpp
	test_xml_serializer.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS ${ALL_SRCS} )

IF( BW_PLATFORM_WINDOWS )
	INCLUDE( "CMakeLists.windows.txt" )
ELSEIF( BW_PLATFORM_MAC )
	INCLUDE( "CMakeLists.mac.txt" )
ENDIF()

BW_ADD_EXECUTABLE( serialization_unit_test ${BLOB_SRCS} )
BW_TARGET_LINK_LIBRARIES( serialization_unit_test PRIVATE
	core_common
	core_reflection
	core_string_utils
	core_unit_test
)

BW_ADD_TOOL_TEST( serialization_unit_test )

BW_PRECOMPILED_HEADER( serialization_unit_test pch.hpp )
BW_PROJECT_CATEGORY( serialization_unit_test "Unit Tests" )
