CMAKE_MINIMUM_REQUIRED( VERSION 3.2.3 )
PROJECT( core_splash )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

SET( ALL_SRCS
	i_splash.hpp
	v0/i_splash_v0.hpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

# Use ADD_CUSTOM_TARGET because this target has no cpp files
ADD_CUSTOM_TARGET( ${PROJECT_NAME} SOURCES
	${ALL_SRCS}
)

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Interfaces" )

