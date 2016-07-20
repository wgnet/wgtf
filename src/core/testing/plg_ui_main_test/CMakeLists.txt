CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_ui_main_test )

ADD_DEFINITIONS( -DNOMINMAX )

INCLUDE( WGToolsCoreProject )
find_package(QT5)

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS(
	-D_CRT_SECURE_NO_WARNINGS
)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( METADATA_IMPL
	pages/metadata/test_page.mpp
	pages/metadata/test_polymorphism.mpp
)
SOURCE_GROUP( "Page\\Metadata" FILES ${METADATA_IMPL} )

SET( PAGE_IMPL
	pages/test_page.hpp
	pages/test_page.cpp
	pages/test_polymorphism.hpp
	pages/test_polymorphism.cpp
)
SOURCE_GROUP( "Page\\Impl" FILES ${PAGE_IMPL} )

SET( INTERFACES
	interfaces/i_datasource.hpp
)
SOURCE_GROUP( "Interfaces" FILES ${INTERFACES} )

BW_QT_ADD_QRC(testing_ui_main resources/testing_ui_main)
BW_GLOB_RESOURCE_FILES()

SET( ALL_SRCS
	test_ui/test_ui.cpp
	test_ui/test_ui.hpp
	test_datasource.cpp
	test_datasource.hpp
	plg_ui_main_test.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_ui_main_test  SHARED
	${ALL_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
	${INTERFACES}
	${PAGE_IMPL}
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

BW_TARGET_LINK_LIBRARIES( plg_ui_main_test PRIVATE
	core_generic_plugin
	core_qt_common
	core_reflection
	reflection_objects_test
	core_command_system
	core_ui_framework
    Qt5::Core
	${PLATFORM_LIBRARIES}
)

BW_PROJECT_CATEGORY( plg_ui_main_test "Plugins" )
