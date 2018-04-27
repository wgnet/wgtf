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

WG_BLOB_SOURCES( BLOB_SRCS ${ALL_SRCS} )

IF( BW_PLATFORM_WINDOWS )
	INCLUDE( "CMakeLists.windows.txt" )
ELSEIF( BW_PLATFORM_MAC )
	INCLUDE( "CMakeLists.mac.txt" )
ENDIF()

BW_ADD_EXECUTABLE( ${PROJECT_NAME} ${BLOB_SRCS} )

IF(BW_PLATFORM_WINDOWS)
	SET(PLATFORM_LIBRARIES shlwapi)
ELSE()
	SET(PLATFORM_LIBRARIES)
ENDIF()

BW_TARGET_LINK_LIBRARIES(  ${PROJECT_NAME} PRIVATE
	core_common
    core_command_system
	core_string_utils
	core_unit_test
	core_serialization_xml

	# external libraries
	${PLATFORM_LIBRARIES}
)

BW_ADD_TOOL_TEST( ${PROJECT_NAME} )

WG_PRECOMPILED_HEADER( ${PROJECT_NAME} pch.hpp )
BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Unit Tests" )
