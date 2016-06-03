CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_perforce )
INCLUDE( WGToolsProject )

FIND_PACKAGE( Perforce "15.2" )

IF( NOT PERFORCE_FOUND )
	# Use ADD_CUSTOM_TARGET because this target has no cpp files
	ADD_CUSTOM_TARGET( ${PROJECT_NAME} COMMENT "${PROJECT_NAME} disabled." )
	BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Disabled" )
	RETURN()
ENDIF()

INCLUDE_DIRECTORIES( ${PERFORCE_INCLUDE_DIRS} )


SET( ALL_SRCS
	perforce_depot_view.cpp
	perforce_depot_view.hpp
	perforce_result.cpp
	perforce_result.hpp
	perforce_version_control.hpp
	perforce_version_control.cpp
	plg_perforce.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )


BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)

BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
	${BLOB_SRCS}
)

BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	core_generic_plugin
	core_string_utils
	${PERFORCE_LIBRARIES}
)

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )
