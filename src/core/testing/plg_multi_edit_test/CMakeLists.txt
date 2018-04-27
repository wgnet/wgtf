CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_multi_edit_test )

ADD_DEFINITIONS( -DNOMINMAX )

INCLUDE( WGToolsCoreProject )

find_package(QT5 ${QT_FIND_PACKAGE_VERSION})

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( ALL_SRCS
	chunk_item_test.cpp
	chunk_item_test.hpp
	multi_edit_test_data.cpp
	multi_edit_test_data.hpp
	metadata/chunk_item_test.mpp
	metadata/multi_edit_test_data.mpp
	plg_multi_edit_test.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_QT_ADD_QRC( plg_multi_edit_test resources/plg_multi_edit_test )

BW_GLOB_RESOURCE_FILES()

BW_ADD_TOOL_PLUGIN( plg_multi_edit_test SHARED
	${ALL_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
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

BW_TARGET_LINK_LIBRARIES( plg_multi_edit_test PRIVATE
	core_generic_plugin
	core_data_model
	${PLATFORM_LIBRARIES}
)

BW_PROJECT_CATEGORY( plg_multi_edit_test "Plugins" )
