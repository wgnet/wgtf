CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_data_model_cmds )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

SET( CMAKE_INCLUDE_CURRENT_DIR ON )

SET( ALL_SRCS
	interfaces/i_item_model_controller.hpp
	interfaces/v0/i_item_model_controller_v0.hpp
	private/commands/insert_default_command.hpp
	private/commands/insert_default_command.cpp
	private/commands/insert_default_command_arg.hpp
	private/commands/insert_default_command_arg.cpp
	private/commands/insert_item_command.hpp
	private/commands/insert_item_command.cpp
	private/commands/insert_item_command_arg.hpp
	private/commands/insert_item_command_arg.cpp
	private/commands/insert_rows_command.hpp
	private/commands/insert_rows_command.cpp
	private/commands/insert_rows_command_arg.hpp
	private/commands/insert_rows_command_arg.cpp
	private/commands/move_item_data_command.hpp
	private/commands/move_item_data_command.cpp
	private/commands/move_item_data_command_arg.hpp
	private/commands/move_item_data_command_arg.cpp
	private/commands/remove_item_command.hpp
	private/commands/remove_item_command.cpp
	private/commands/remove_item_command_arg.hpp
	private/commands/remove_item_command_arg.cpp
	private/commands/remove_rows_command.hpp
	private/commands/remove_rows_command.cpp
	private/commands/remove_rows_command_arg.hpp
	private/commands/remove_rows_command_arg.cpp
	private/commands/set_item_data_command.hpp
	private/commands/set_item_data_command.cpp
	private/commands/set_item_data_command_arg.hpp
	private/commands/set_item_data_command_arg.cpp
	private/commands/set_model_data_command.hpp
	private/commands/set_model_data_command.cpp
	private/commands/set_model_data_command_arg.hpp
	private/commands/set_model_data_command_arg.cpp
	private/commands/metadata/insert_default_command_arg.mpp
	private/commands/metadata/insert_item_command_arg.mpp
	private/commands/metadata/insert_rows_command_arg.mpp
	private/commands/metadata/move_item_data_command_arg.mpp
	private/commands/metadata/remove_item_command_arg.mpp
	private/commands/metadata/remove_rows_command_arg.mpp
	private/commands/metadata/set_item_data_command_arg.mpp
	private/commands/metadata/set_model_data_command_arg.mpp
	private/item_model_controller.cpp
	private/item_model_controller.hpp
)

WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )


WG_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_LIBRARY( ${PROJECT_NAME} ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} INTERFACE
	core_data_model
	core_command_system
)

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "WGT Libs/Data Model" )

