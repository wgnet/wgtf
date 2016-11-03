CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_dialog_test )

ADD_DEFINITIONS( -DNOMINMAX )

INCLUDE( WGToolsCoreProject )

find_package(QT5)

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS(
	-D_CRT_SECURE_NO_WARNINGS
)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( ALL_SRCS
	dialog_test_model.hpp
	dialog_test_model.cpp
    dialog_reflected_data.hpp
    dialog_reflected_data.cpp
    dialog_reflected_tree_model.hpp
    dialog_reflected_tree_model.cpp
    metadata/dialog_reflected_data.mpp
    metadata/dialog_reflected_tree_model.mpp
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
	core_qt_common
	core_ui_framework
    Qt5::Core
)

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )
