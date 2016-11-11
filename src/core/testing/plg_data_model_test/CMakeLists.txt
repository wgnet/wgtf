CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_data_model_test )

INCLUDE( WGToolsCoreProject )
find_package(QT5)

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS(
	-D_CRT_SECURE_NO_WARNINGS
)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( ALL_SRCS
	custom_model_interface_test.cpp
	custom_model_interface_test.hpp
	plg_data_model_test.cpp
)
WG_AUTO_SOURCE_GROUPS( "" FILES ${ALL_SRCS} )

BW_QT_ADD_QRC(plg_data_model_test resources/plg_data_model_test)
BW_GLOB_RESOURCE_FILES()

BW_ADD_TOOL_PLUGIN( plg_data_model_test SHARED
	${ALL_SRCS}
	${COMPILED_RESOURCES}
	${RESOURCE_FILES}
)

BW_TARGET_LINK_LIBRARIES( plg_data_model_test PRIVATE
	core_generic_plugin
	core_reflection
	core_data_model
)

BW_PROJECT_CATEGORY( plg_data_model_test "Plugins" )
