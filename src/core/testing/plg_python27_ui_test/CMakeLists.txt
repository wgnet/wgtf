CMAKE_MINIMUM_REQUIRED( VERSION 3.2.3 )
PROJECT( plg_python27_ui_test )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGPython27Common )

FIND_PACKAGE( QT5 )

SET( METADATA_SRCS
    metadata/ui_test_panel_context.mpp
)
SOURCE_GROUP( "MetaData" FILES ${METADATA_SRCS} )

SET( PYTHON_FILES
	scripts/test_objects/__init__.py
)
SOURCE_GROUP( "Python" FILES ${PYTHON_FILES} )

SET( QML_FILES
	resources/Python27UITest/PythonObjectTestPanel.qml
)
SOURCE_GROUP( "QML" FILES ${QML_FILES} )
BW_QT_ADD_QRC( Python27UITest resources/Python27UITest )
BW_GLOB_RESOURCE_FILES()

SET( ALL_SRCS
	plg_python27_ui_test.cpp
	python_panel.cpp
	python_panel.hpp
	ui_test_panel_context.cpp
	ui_test_panel_context.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )


BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
	${BLOB_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
	${METADATA_SRCS}
	${QML_FILES}
	${PYTHON_FILES}
)

BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	core_generic_plugin
	core_qt_common
	core_ui_framework
	Qt5::Core
)

WG_DEPLOY_PYTHON( ${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/scripts scripts/${PROJECT_NAME} )

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )
