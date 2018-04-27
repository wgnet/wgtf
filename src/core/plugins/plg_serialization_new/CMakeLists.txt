CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_serialization_new )

FIND_PACKAGE(tinyxml2)

INCLUDE( WGToolsCoreProject )

SET ( ALL_SRCS
	plg_serialization_new.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
	${ALL_SRCS}
)

BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	core_generic_plugin
	tinyxml2
	core_serialization_new
)

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )