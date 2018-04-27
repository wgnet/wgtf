CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_toolbox_test )

ADD_DEFINITIONS( -DNOMINMAX )

INCLUDE( WGToolsCoreProject )

find_package(QT5 ${QT_FIND_PACKAGE_VERSION})

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( ALL_SRCS
	test_tool.hpp
	plg_toolbox_test.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_QT_ADD_QRC(plg_toolbox_test resources)
BW_GLOB_RESOURCE_FILES()

BW_ADD_TOOL_PLUGIN( plg_toolbox_test SHARED
	${ALL_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( plg_toolbox_test PRIVATE
	core_generic_plugin
	core_object
)

BW_PROJECT_CATEGORY( plg_toolbox_test "Plugins" )
