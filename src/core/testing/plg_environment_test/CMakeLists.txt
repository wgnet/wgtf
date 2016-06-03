CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_environment_test )

ADD_DEFINITIONS( -DNOMINMAX )

INCLUDE( WGToolsCoreProject )
find_package(QT5)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( METADATA_IMPL
	project/metadata/project.mpp
)
SOURCE_GROUP( "Project\\Metadata" FILES ${METADATA_IMPL} )

SET( PROJECT_IMPL
	project/project.hpp
	project/project.cpp
)
SOURCE_GROUP( "Project\\Impl" FILES ${PROJECT_IMPL} )


BW_QT_ADD_QRC(TestingProjectControl resources/TestingProjectControl)
BW_GLOB_RESOURCE_FILES()

SET( ALL_SRCS
	plg_environment_test.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_environment_test  SHARED
	${ALL_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
	${PROJECT_IMPL}
	${METADATA_IMPL}
)

IF( BW_PLATFORM_WINDOWS )
	SET( PLATFORM_LIBRARIES
		shlwapi)
	SET( PLUGIN_CONFIG_FOLDER plugins)
ELSEIF( BW_PLATFORM_MAC )
	SET(PLATFORM_LIBRARIES
		core_common)
	SET( PLUGIN_CONFIG_FOLDER ../Resources/plugins )

	SET( CMAKE_EXE_LINKER_FLAGS
		"${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,@loader_path/../Frameworks"
	)

ENDIF()

BW_TARGET_LINK_LIBRARIES( plg_environment_test PRIVATE
	core_generic_plugin
	core_qt_common
	core_reflection
	core_command_system
	core_serialization
	core_ui_framework
    Qt5::Core
	${PLATFORM_LIBRARIES}
)

BW_PROJECT_CATEGORY( plg_environment_test "Plugins" )
