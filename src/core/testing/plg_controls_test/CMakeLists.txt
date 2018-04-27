CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_controls_test )

INCLUDE( WGToolsCoreProject )

FIND_PACKAGE( QT5 ${QT_FIND_PACKAGE_VERSION} )


SET( ALL_SRCS
	controls_test_panel.hpp
	controls_test_panel.cpp
	plg_controls_test.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

SET(METADATA
	metadata/controls_test_panel.mpp
)
SOURCE_GROUP("metadata" FILES ${METADATA})

WG_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
	${METADATA}
)

BW_QT_ADD_QRC( WGControlsTest resources/plg_controls_test )
BW_GLOB_RESOURCE_FILES()

BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
	${BLOB_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	core_generic_plugin
	core_data_model
)

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )
