CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_grid_editor_test )

ADD_DEFINITIONS( -DNOMINMAX )

INCLUDE( WGToolsCoreProject )

find_package(QT5)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( ALL_SRCS
	test_grid_item.cpp
	test_grid_item.hpp
	test_grid_model.cpp
	test_grid_model.hpp
	plg_grid_test.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_QT_ADD_QRC( plg_grid_editor_test resources/plg_grid_editor_test )

BW_GLOB_RESOURCE_FILES()

BW_ADD_TOOL_PLUGIN( plg_grid_editor_test SHARED
	${ALL_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( plg_grid_editor_test PRIVATE
	core_generic_plugin
	core_qt_common
	core_ui_framework
    Qt5::Core
)

BW_PROJECT_CATEGORY( plg_grid_editor_test "Plugins" )
