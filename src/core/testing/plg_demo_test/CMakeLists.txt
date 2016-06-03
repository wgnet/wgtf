CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_demo_test )

INCLUDE( WGToolsCoreProject )

FIND_PACKAGE( QT5 )

SET( METADATA_SRCS
	metadata/demo_objects_fix_mixin.mpp
	metadata/demo_objects.mpp
)
SOURCE_GROUP( "" FILES ${METADATA_SRCS} )

SET( ALL_SRCS
	plg_demo_test.cpp
	demo_objects.cpp
	demo_objects.hpp
	demo_objects_fix_mixin.hpp
	demo_objects_fix_mixin.cpp
	object_selection_helper.cpp
	object_selection_helper.hpp
	tinyxml2.cpp
	tinyxml2.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_QT_ADD_QRC( DemoTest resources/DemoTest )
BW_GLOB_RESOURCE_FILES()

FILE( GLOB MODEL_FILES resources/sceneModel0.xml resources/sceneModel1.xml )
SOURCE_GROUP( "Model Files" FILES ${MODEL_FILES} )
BW_CUSTOM_COPY_TO_PROJECT_OUTPUT( .. "${MODEL_FILES}" )

BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
	${ALL_SRCS}
	${METADATA_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
	${MODEL_FILES}
)

BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	core_generic_plugin
	core_qt_common
	core_ui_framework
	Qt5::Core
	Qt5::Quick
)

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )

