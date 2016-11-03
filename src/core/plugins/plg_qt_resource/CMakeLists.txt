CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_qt_resource )

INCLUDE( WGToolsCoreProject )
FIND_PACKAGE( QT5 )

SET( ALL_SRCS
    plg_qt_resource.cpp
    qt_resource_system.cpp
    qt_resource_system.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
    ${ALL_SRCS}
)

BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
	${BLOB_SRCS}
)


BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
    core_dependency_system
    core_logging
    core_generic_plugin
    core_serialization
    wg_types
    Qt5::Core
)

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )

