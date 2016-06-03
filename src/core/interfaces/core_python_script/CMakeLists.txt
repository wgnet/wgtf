CMAKE_MINIMUM_REQUIRED( VERSION 3.2.3 )
PROJECT( core_python_script )

INCLUDE( WGToolsCoreProject )


SET( V0_INTERFACES
	v0/i_scripting_engine_v0.hpp
)
SOURCE_GROUP( "v0" FILES ${V0_INTERFACES} )


SET( ALL_SRCS
	i_scripting_engine.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )


# Use ADD_CUSTOM_TARGET because this target has no cpp files
ADD_CUSTOM_TARGET( ${PROJECT_NAME} SOURCES
	${V0_INTERFACES}
	${ALL_SRCS}
)
 
BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Interfaces" )

