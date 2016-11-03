CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_automation )

INCLUDE( WGToolsCoreProject )

SET( V0_INTERFACES
	interfaces/v0/automation_interface_v0.hpp
	interfaces/v0/automation_interface_v1.hpp
)
SOURCE_GROUP( "v0" FILES ${V0_INTERFACES} )


SET( ALL_SRCS
	interfaces/automation_interface.hpp
)

WG_AUTO_SOURCE_GROUPS(${ALL_SRCS})

BW_BLOB_SOURCES( BLOB_SRCS
	${V0_INTERFACES}
	${ALL_SRCS}
)

# Use ADD_CUSTOM_TARGET because this target has no cpp files
ADD_CUSTOM_TARGET( core_automation SOURCES
	${BLOB_SRCS}
)
 
BW_PROJECT_CATEGORY( core_automation "Wargaming Libs" )

