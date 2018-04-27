CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_reflection_utils )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( ALL_SRCS
	commands/custom_command.hpp
	commands/custom_command.cpp
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
	commands/metadata/custom_command.mpp
	commands/metadata/invoke_reflected_method_command.mpp
	commands/metadata/set_reflectedproperty_command.mpp
	commands/metadata/reflected_collection_insert_command.mpp
	commands/metadata/reflected_collection_erase_command.mpp
	reflection_controller.cpp
	reflection_controller.hpp
)

WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )


WG_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_LIBRARY( core_reflection_utils ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( core_reflection_utils INTERFACE
	core_reflection
	core_command_system
	wgtf_types
)

BW_PROJECT_CATEGORY( core_reflection_utils "WGT Libs/Reflection" )

