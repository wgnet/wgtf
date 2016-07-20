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
	layout_hint.cpp
	layout_hint.hpp
	layout_manager.cpp
	layout_manager.hpp
	layout_tags.hpp
	simple_component_provider.hpp
)
SOURCE_GROUP( "Source" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_LIBRARY( core_ui_framework ${BLOB_SRCS} )

BW_PROJECT_CATEGORY( core_ui_framework "WGT Libs" )

