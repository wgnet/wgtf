CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

PROJECT( core_base_editor )

SET( ALL_SRCS
	base_editor.cpp
	base_editor.hpp
	log_listener.hpp
	log_listener.cpp
	resource_log_listener.hpp
	resource_log_listener.cpp
	interfaces/i_loading_event_source.hpp
)

WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

WG_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_LIBRARY( core_base_editor ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( core_base_editor
INTERFACE
core_common
core_logging_system
)


BW_PROJECT_CATEGORY( core_base_editor "WGT Libs" )
