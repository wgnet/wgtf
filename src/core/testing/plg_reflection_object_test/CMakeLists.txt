CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_reflection_object_test )

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
	${QML_SRCS}
	plg_reflection_object_test.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_reflection_object_test SHARED
	${ALL_SRCS}
)

BW_TARGET_LINK_LIBRARIES( plg_reflection_object_test PRIVATE
	core_generic_plugin
	core_reflection
	reflection_objects_test
    Qt5::Core
)

BW_PROJECT_CATEGORY( plg_reflection_object_test "Plugins" )
