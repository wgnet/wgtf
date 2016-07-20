CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_reflection_utils )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( COMMAND_SRCS
	commands/invoke_reflected_method_command.hpp
	commands/invoke_reflected_method_command.cpp
	commands/set_reflectedproperty_command.hpp
	commands/set_reflectedproperty_command.cpp
	commands/reflected_collection_insert_command.hpp
	commands/reflected_collection_insert_command.cpp
	commands/reflected_collection_erase_command.hpp
	commands/reflected_collection_erase_command.cpp
	commands/reflectedproperty_undoredo_helper.hpp
	commands/reflectedproperty_undoredo_helper.cpp
)
SOURCE_GROUP( "Commands" FILES ${COMMAND_SRCS} )

SET( COMMAND_METADATA_SRCS
	commands/metadata/invoke_reflected_method_command.mpp
	commands/metadata/set_reflectedproperty_command.mpp
	commands/metadata/reflected_collection_insert_command.mpp
	commands/metadata/reflected_collection_erase_command.mpp
)
SOURCE_GROUP( "Commands\\MetaData" FILES ${COMMAND_METADATA_SRCS} )

SET( SERIALIZER_SRCS
	serializer/reflection_serializer.cpp
	serializer/reflection_serializer.hpp
)
SOURCE_GROUP( "Serializers" FILES ${SERIALIZER_SRCS} )

	
SET( ALL_SRCS
	reflected_types.hpp
	reflected_types.cpp
	reflection_controller.cpp
	reflection_controller.hpp
)
SOURCE_GROUP( "Source" FILES ${ALL_SRCS} )


BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
	${COMMAND_SRCS}
	${COMMAND_METADATA_SRCS}
	${SERIALIZER_SRCS}
)
BW_ADD_LIBRARY( core_reflection_utils ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( core_reflection_utils INTERFACE
	core_reflection
	core_command_system
	wg_types
)

BW_PROJECT_CATEGORY( core_reflection_utils "WGT Libs/Reflection" )

