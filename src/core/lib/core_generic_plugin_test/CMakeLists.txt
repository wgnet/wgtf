CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_generic_plugin_test )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

SET( ALL_SRCS
	plugin_memory_allocator.cpp
	plugin_memory_allocator.hpp
	memory_plugin_context_creator.cpp
	memory_plugin_context_creator.hpp
	test_plugin_loader.hpp
	test_plugin_loader.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )


BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_LIBRARY( core_generic_plugin_test ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( core_generic_plugin_test INTERFACE
	core_generic_plugin_manager
	wg_memory
)

BW_PROJECT_CATEGORY( core_generic_plugin_test "Unit Tests" )
