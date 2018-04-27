CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_test_active_filters )

INCLUDE( WGStandardProject )
FIND_PACKAGE( QT5 ${QT_FIND_PACKAGE_VERSION})

# Find includes in corresponding build directories
SET( CMAKE_INCLUDE_CURRENT_DIR ON )

SET( ALL_SRCS
	metadata/active_filters_test_view_model.mpp
	active_filters_test_view_model.cpp
	active_filters_test_view_model.hpp
	plugin_main.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_QT_ADD_QRC( TestActiveFilters resources/TestActiveFilters )
BW_GLOB_RESOURCE_FILES()

BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
	${ALL_SRCS}
	${COMPILED_RESOURCES}
	${RESOURCE_FILES}
)

BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	core_generic_plugin
	core_reflection
	data_model_test
)

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )
