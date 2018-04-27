CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_data_model_unit_test )

INCLUDE( WGToolsCoreProject )

SET( ALL_SRCS
	main.cpp
	pch.cpp
	pch.hpp
	test_abstract_item_model.hpp
	test_abstract_item_model.cpp
	test_collection_model.cpp
	test_data_model_objects.hpp
	test_data_model_objects.cpp
	test_data_model.cpp
	test_data_model_fixture.hpp
	test_data_model_fixture.cpp
	test_string_data.hpp
	test_string_data.cpp
    test_variant_list.cpp
)

WG_BLOB_SOURCES( BLOB_SRCS ${ALL_SRCS} )
BW_ADD_EXECUTABLE( ${PROJECT_NAME} ${BLOB_SRCS} )

IF( BW_PLATFORM_WINDOWS )
	SET( PLATFORM_LIBRARIES shlwapi )
ELSEIF( BW_PLATFORM_MAC )
	SET( PLATFORM_LIBRARIES core_common )
ENDIF()

BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	core_data_model
	core_unit_test

	# external libraries
	${PLATFORM_LIBRARIES}    
)

BW_ADD_TOOL_TEST( ${PROJECT_NAME} )

WG_PRECOMPILED_HEADER( ${PROJECT_NAME} pch.hpp )
BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Unit Tests" )

