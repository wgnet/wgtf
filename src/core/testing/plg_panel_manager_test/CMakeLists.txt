CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_panel_manager_test )

INCLUDE( WGStandardProject )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS )

ADD_DEFINITIONS(
	-DCSTDMF_IMPORT
)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( ALL_SRCS
	plg_panel_manager_test.cpp
	test_asset_presentation_provider.cpp
	test_asset_presentation_provider.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

FILE( GLOB TEST_RESOURCES resources/test_* )
SOURCE_GROUP( "Test Resources" FILES ${TEST_RESOURCES} )
BW_CUSTOM_COPY_TO_PROJECT_OUTPUT( "test_resources" "${TEST_RESOURCES}" )

BW_ADD_TOOL_PLUGIN( plg_panel_manager_test SHARED
	${ALL_SRCS}
	${TEST_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( plg_panel_manager_test PRIVATE
	core_generic_plugin
	core_data_model
	core_string_utils
)

BW_PROJECT_CATEGORY( plg_panel_manager_test "Plugins" )
