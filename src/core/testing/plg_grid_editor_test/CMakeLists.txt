CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_grid_editor_test )

ADD_DEFINITIONS( -DNOMINMAX )

INCLUDE( WGToolsCoreProject )

find_package(QT5)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( ALL_SRCS
	grid_editor_test_data.cpp
	grid_editor_test_data.hpp
	grid_editor_test_data.mpp
	plg_grid_test.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_QT_ADD_QRC( plg_grid_editor_test resources/plg_grid_editor_test )

BW_GLOB_RESOURCE_FILES()

BW_ADD_TOOL_PLUGIN( plg_grid_editor_test SHARED
	${ALL_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( plg_grid_editor_test PRIVATE
	core_generic_plugin
	core_data_model
)

BW_PROJECT_CATEGORY( plg_grid_editor_test "Plugins" )
