CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_data_model_cmds )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

SET( INTERFACES_SRCS
	interfaces/i_item_model_controller.hpp
	interfaces/v0/i_item_model_controller_v0.hpp
)
SOURCE_GROUP( "Interfaces" FILES ${INTERFACES_SRCS} )

SET( COMMAND_SRCS
	commands/set_item_data_command.hpp
	commands/set_item_data_command.cpp
	commands/set_item_data_command_arg.hpp
	commands/set_item_data_command_arg.cpp
)
SOURCE_GROUP( "Commands" FILES ${COMMAND_SRCS} )

SET( COMMAND_METADATA_SRCS
	commands/metadata/set_item_data_command.mpp
)
SOURCE_GROUP( "Commands\\MetaData" FILES ${COMMAND_METADATA_SRCS} )

SET( ALL_SRCS
	item_model_controller.cpp
	item_model_controller.hpp
)
SOURCE_GROUP( "Source" FILES ${ALL_SRCS} )


BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
	${COMMAND_SRCS}
	${COMMAND_METADATA_SRCS}
	${INTERFACES_SRCS}
)
BW_ADD_LIBRARY( ${PROJECT_NAME} ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} INTERFACE
	core_data_model
	core_command_system
)

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "WGT Libs/Data Model" )

