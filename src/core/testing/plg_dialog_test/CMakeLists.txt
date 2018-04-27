CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_dialog_test )

ADD_DEFINITIONS( -DNOMINMAX )

INCLUDE( WGToolsCoreProject )

find_package(QT5 ${QT_FIND_PACKAGE_VERSION})

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS(
	-D_CRT_SECURE_NO_WARNINGS
)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( ALL_SRCS
	dialog_test_panel.hpp
	dialog_test_panel.cpp
	custom_dialog_model.hpp
    dialog_reflected_data.hpp
    dialog_reflected_data.cpp
    metadata/dialog_reflected_data.mpp
	${PROJECT_NAME}.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_QT_ADD_QRC(${PROJECT_NAME} resources/${PROJECT_NAME})
BW_GLOB_RESOURCE_FILES()

BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
	${ALL_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
)

BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	core_generic_plugin
	core_data_model
)

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )
