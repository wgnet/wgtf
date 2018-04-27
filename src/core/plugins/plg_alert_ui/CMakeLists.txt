CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_alert_ui )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

find_package(QT5 ${QT_FIND_PACKAGE_VERSION})
find_package( Qt5Quick REQUIRED )

BW_QT_ADD_QRC(plg_alert_ui resources/plg_alert_ui)
BW_GLOB_RESOURCE_FILES()

SET( ALL_SRCS
	alert_models.cpp
	alert_models.hpp
	metadata/alert_models.mpp
	plg_alert_ui.cpp
	popup_alert_presenter.cpp
	popup_alert_presenter.hpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

WG_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)

BW_ADD_TOOL_PLUGIN( plg_alert_ui SHARED
	${BLOB_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( plg_alert_ui PRIVATE
	core_generic_plugin
	core_object
	core_data_model
	core_reflection
	core_logging_system
)

BW_PROJECT_CATEGORY( plg_alert_ui "Plugins" )
