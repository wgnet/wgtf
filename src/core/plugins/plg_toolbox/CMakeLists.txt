CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_toolbox )

INCLUDE( WGToolsProject )
find_package(QT5 ${QT_FIND_PACKAGE_VERSION})

# Find includes in corresponding build directories
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

FILE(GLOB_RECURSE QML_SRCS *.qml *.js *.ui *.xml)
SET(QT_RESOURCE_FILES
	${QML_SRCS}
)

# feels like there should be a better was of setting up dependencies, but unfortunately it appears
# that this is the only way of doing it :(
INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/core/interfaces)

SOURCE_GROUP("Qt Resources" FILES ${QT_RESOURCE_FILES})

SOURCE_GROUP("Resource Files" FILES ${RESOURCE_FILES} )
SOURCE_GROUP("Compiled Resources" FILES ${COMPILED_RESOURCES})

BW_QT_ADD_QRC(plg_toolbox resources)
BW_GLOB_RESOURCE_FILES()

SET( ALL_SRCS
	toolbox.cpp
	toolbox.hpp
	toolbox_panel.cpp
	toolbox_panel.hpp
	plg_toolbox.cpp
	reflection_auto_reg.mpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

SET( INTERFACES
	${BW_SOURCE_DIR}/core/interfaces/toolbox/i_tool.hpp
	${BW_SOURCE_DIR}/core/interfaces/toolbox/i_toolbox.hpp
)
SOURCE_GROUP("Interfaces" FILES ${INTERFACES})

SET(METADATA
	metadata/i_tool.mpp
)
SOURCE_GROUP("Metadata" FILES ${METADATA})

BW_ADD_TOOL_PLUGIN( plg_toolbox SHARED
	${ALL_SRCS}
	${INTERFACES}
	${COMPILED_RESOURCES}
	${QT_RESOURCE_FILES}
	${QT_ICON_SRCS}
	${RESOURCE_FILES}
	${METADATA}
)

BW_TARGET_LINK_LIBRARIES( plg_toolbox PRIVATE
	core_generic_plugin
    core_serialization_xml
    core_data_model
)

BW_PROJECT_CATEGORY( plg_toolbox "Plugins" )
