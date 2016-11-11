CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_panel_manager_test )

INCLUDE( WGStandardProject )

FIND_PACKAGE(QT5)

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS )

ADD_DEFINITIONS(
	-DCSTDMF_IMPORT
)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Add Qt5Gui to enable clipboard for CopyFolderPath
INCLUDE_DIRECTORIES(${Qt5Gui_INCLUDE_DIRS})

SET( ALL_SRCS
	plg_panel_manager_test.cpp
	test_asset_browser_folder_context_menu.cpp
	test_asset_browser_folder_context_menu.hpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_QT_ADD_QRC(plg_panel_manager_test resources)
BW_GLOB_RESOURCE_FILES()

FILE( GLOB PLUGIN_RESOURCES resources/*.png )
BW_COPY_PLUGIN_RESOURCES( "${PLUGIN_RESOURCES}" )

BW_ADD_TOOL_PLUGIN( plg_panel_manager_test SHARED
	${ALL_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
	${PLUGIN_RESOURCES}        
)

BW_TARGET_LINK_LIBRARIES( plg_panel_manager_test PRIVATE
	core_generic_plugin
	core_data_model
	core_string_utils
	core_ui_framework
	Qt5::Gui
)

BW_PROJECT_CATEGORY( plg_panel_manager_test "Plugins" )
