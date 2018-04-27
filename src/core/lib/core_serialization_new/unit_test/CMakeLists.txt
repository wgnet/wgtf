CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( serialization_new_unit_test )

INCLUDE( WGToolsCoreProject )

SET( ALL_SRCS
	main.cpp
	pch.cpp
	pch.hpp
	test_serialization_new.cpp
	nstest_big_class.hpp
	nstest_big_class.cpp
	nstest_big_class_handler.hpp
	nstest_big_class_handler.cpp
	nstest_small_class.hpp
	nstest_small_class.cpp
	nstest_small_class_handler.hpp
	nstest_small_class_handler.cpp
	nstest_reflected_classes.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

WG_BLOB_SOURCES( BLOB_SRCS ${ALL_SRCS} )

BW_ADD_EXECUTABLE( ${PROJECT_NAME} ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES(  ${PROJECT_NAME} PRIVATE
	core_common
	core_serialization_new
	core_unit_test
	tf_types
)

BW_ADD_TOOL_TEST( ${PROJECT_NAME} )

WG_PRECOMPILED_HEADER( ${PROJECT_NAME} pch.hpp )
BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Unit Tests" )
