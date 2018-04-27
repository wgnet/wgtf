CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_ui_main_test )

ADD_DEFINITIONS( -DNOMINMAX )

INCLUDE( WGToolsCoreProject )
find_package(QT5 ${QT_FIND_PACKAGE_VERSION})

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( QML_FILES
	resources/testing_ui_main/actions.xml
	resources/testing_ui_main/test_property_tree_panel.qml
)
SOURCE_GROUP( "QML" FILES ${QML_FILES} )
BW_QT_ADD_QRC(testing_ui_main resources/testing_ui_main)
BW_GLOB_RESOURCE_FILES()

SET( ALL_SRCS
	pages/metadata/test_page.mpp
	pages/metadata/test_polymorphism.mpp
	pages/test_page.hpp
	pages/test_page.cpp
	pages/test_polymorphism.hpp
	pages/test_polymorphism.cpp
	interfaces/i_datasource.hpp
	test_ui/context.cpp
	test_ui/context.hpp
	test_ui/metadata/context.mpp
	test_ui/test_ui.cpp
	test_ui/test_ui.hpp
	test_ui/test_viewport.cpp
	test_ui/test_viewport.hpp
	test_datasource.cpp
	test_datasource.hpp
	plg_ui_main_test.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_ui_main_test  SHARED
	${ALL_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
	${QML_FILES}
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
	core_data_model
	core_reflection
	reflection_objects_test
	core_viewport
	core_serialization_xml
	${PLATFORM_LIBRARIES}
    Qt5::Core
    Qt5::Gui
    Qt5::QuickWidgets
    Qt5::Quick	
)

BW_PROJECT_CATEGORY( plg_ui_main_test "Plugins" )
