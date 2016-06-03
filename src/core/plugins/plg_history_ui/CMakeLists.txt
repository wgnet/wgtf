CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_history_ui )

INCLUDE( WGToolsCoreProject )
find_package(QT5)

SET( METADATA_SRCS
	metadata/history_object.mpp
    metadata/display_object.mpp
)
SOURCE_GROUP( "MetaData" FILES ${METADATA_SRCS} )

SET( ALL_SRCS
	history_object.cpp
	history_object.hpp
    display_object.cpp
    display_object.hpp
	plg_history_ui.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_QT_ADD_QRC(WGHistory resources/WGHistory)
BW_GLOB_RESOURCE_FILES()

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
	${METADATA_SRCS}
)
BW_ADD_TOOL_PLUGIN( plg_history_ui SHARED
	${BLOB_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( plg_history_ui PRIVATE
	core_command_system
	core_reflection
	core_qt_common
	core_generic_plugin
    Qt5::Core
    Qt5::Quick
)

BW_PROJECT_CATEGORY( plg_history_ui "Plugins" )

