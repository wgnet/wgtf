CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_command_system )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

SET( ALL_SRCS
	i_command_event_listener.hpp
	compound_command.hpp 
	compound_command.cpp
	command_instance.hpp 
	command_instance.cpp
	command_manager.hpp 
	command_manager.cpp 
	i_command_manager.hpp 
	command.hpp 
	command.cpp
	batch_command.hpp
	batch_command.cpp
	batch_command.mpp
	undo_redo_command.hpp
	undo_redo_command.cpp
	undo_redo_command.mpp
	undo_redo_data.hpp
	custom_undo_redo_data.hpp
	custom_undo_redo_data.cpp
	reflection_undo_redo_data.hpp
	reflection_undo_redo_data.cpp
)

WG_AUTO_SOURCE_GROUPS(${ALL_SRCS} )


WG_BLOB_SOURCES( BLOB_SRCS 
	${ALL_SRCS} 
)
BW_ADD_LIBRARY( core_command_system ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( core_command_system INTERFACE
	core_common
	core_object
	core_reflection
	core_reflection_utils
	core_data_model
	core_logging
)

BW_PROJECT_CATEGORY( core_command_system "WGT Libs" )
