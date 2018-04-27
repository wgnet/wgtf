CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_ui_framework)

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

SET( ALL_SRCS
    action_manager.cpp
    action_manager.hpp
	generic_component_provider.hpp
	i_action.hpp
	i_component.hpp
	i_component_provider.hpp
	i_cursor.hpp
	i_dialog.hpp
	i_menu.hpp
	i_menu_handler.cpp
	i_menu_handler.hpp
	i_region.hpp
	i_status_bar.hpp
	i_ui_application.hpp
	i_ui_framework.hpp
	i_view.hpp
	i_window.hpp
	i_preferences.hpp
	i_progress_dialog.hpp
    i_thumbnail_provider.hpp
	layout_hint.cpp
	layout_hint.hpp
	layout_manager.cpp
	layout_manager.hpp
	layout_tags.hpp
	palette.hpp
	simple_component_provider.hpp
	string_component_provider.hpp
	interfaces/i_action_manager.hpp
	interfaces/i_view_creator.hpp
	i_system_tray_icon.hpp
)

WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

WG_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_LIBRARY( core_ui_framework ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( core_ui_framework INTERFACE
	core_object
)

BW_PROJECT_CATEGORY( core_ui_framework "WGT Libs" )

