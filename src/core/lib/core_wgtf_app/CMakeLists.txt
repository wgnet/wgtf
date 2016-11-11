CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_wgtf_app )

ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS )

SET( ALL_SRCS
	command_line_parser.hpp
	command_line_parser.cpp
	memory_plugin_context_creator.hpp
	memory_plugin_context_creator.cpp
	plugin_memory_allocator.hpp
	plugin_memory_allocator.cpp
)

WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_LIBRARY( core_wgtf_app ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( core_wgtf_app PRIVATE
)

BW_PROJECT_CATEGORY( core_wgtf_app "WGT Libs" )
