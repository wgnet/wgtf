CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_grabber_manager_test )

INCLUDE( WGToolsCoreProject )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( ALL_SRCS
	${PROJECT_NAME}.cpp
	unique_settings_data.hpp
	static_settings_data.hpp
	metadata/unique_settings_data.mpp
	metadata/static_settings_data.mpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
	${ALL_SRCS}
)

BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	core_reflection
	core_object
	core_generic_plugin
)

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )
