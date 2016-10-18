CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_automation )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

SET( ALL_SRCS
	plg_automation.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_automation SHARED
	${ALL_SRCS}
)

BW_TARGET_LINK_LIBRARIES( plg_automation PRIVATE
	core_generic_plugin
)

BW_PROJECT_CATEGORY( plg_automation "Plugins" )

