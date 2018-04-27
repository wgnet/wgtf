CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_table_model_test )

ADD_DEFINITIONS( -DNOMINMAX )

INCLUDE( WGToolsCoreProject )

find_package(QT5 ${QT_FIND_PACKAGE_VERSION})

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( ALL_SRCS
	test_table_item.cpp
	test_table_item.hpp
	test_table_model.cpp
	test_table_model.hpp
	plg_table_model_test.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_QT_ADD_QRC( plg_table_model_test resources/plg_table_model_test )

BW_GLOB_RESOURCE_FILES()

BW_ADD_TOOL_PLUGIN( plg_table_model_test SHARED
	${ALL_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( plg_table_model_test PRIVATE
	core_generic_plugin
	core_data_model
)

BW_PROJECT_CATEGORY( plg_table_model_test "Plugins" )
