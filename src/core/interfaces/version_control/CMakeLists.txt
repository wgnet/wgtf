CMAKE_MINIMUM_REQUIRED( VERSION 3.2.3 )
PROJECT( version_control )

INCLUDE( WGToolsCoreProject )


SET( ALL_SRCS
	i_version_control.hpp
	i_depot_view.hpp
	i_result.hpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )


# Use ADD_CUSTOM_TARGET because this target has no cpp files
ADD_CUSTOM_TARGET( ${PROJECT_NAME} SOURCES
	${ALL_SRCS}
)
 
BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Interfaces" )

