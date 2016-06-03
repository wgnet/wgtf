CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_plugin1_test )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( METADATA_SRCS
	metadata/plugin_objects.mpp
)
SOURCE_GROUP( "MetaData" FILES ${METADATA_SRCS} )

SET( ALL_SRCS
	plg_plugin1_test.cpp
	plugin_objects.hpp
	plugin_objects.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
	${METADATA_SRCS}
)
BW_ADD_TOOL_TEST_PLUGIN( plg_plugin1_test SHARED
	${BLOB_SRCS}
)

BW_TARGET_LINK_LIBRARIES( plg_plugin1_test PRIVATE
	core_generic_plugin
	core_reflection
)

