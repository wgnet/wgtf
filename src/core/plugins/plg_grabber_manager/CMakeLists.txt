CMAKE_MINIMUM_REQUIRED( VERSION 2.8.8 )
PROJECT( plg_grabber_manager)

INCLUDE( WGToolsProject )
find_package(QT5 ${QT_FIND_PACKAGE_VERSION})

# Find includes in corresponding build directories
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

INCLUDE_DIRECTORIES(${BW_SOURCE_DIR}/core/interfaces)
INCLUDE_DIRECTORIES( ${WG_TOOLS_SOURCE_DIR}/core )

SET( INTERFACES
	../../interfaces/grabber_manager/i_grabber.hpp
	../../interfaces/grabber_manager/i_grabber_manager.hpp
)
SOURCE_GROUP("Interfaces" FILES ${INTERFACES})

SET( PLG_SRCS
	${PROJECT_NAME}.cpp
	grabber_manager.cpp
	grabber_manager.hpp
	simple_grabber.hpp
	grabber_utils.hpp
)
SOURCE_GROUP( "" FILES ${PLG_SRCS} )
 
BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
	${ALL_SRCS}
	${PLG_SRCS}
	${INTERFACES}
)
 
BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	core_generic_plugin
	core_object
	core_reflection
	core_environment_system
	core_dependency_system
	core_common
	core_generic_plugin
	core_view_model
	core_data_model
	core_ui_framework
	core_serialization_xml
	Qt5::Core
	)

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )
