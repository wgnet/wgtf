CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( generic_app )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGProjectUserFile )

SET( APP_CODE_SRCS
	app/main.cpp
    app/command_line_parser.hpp
    app/command_line_parser.cpp
	app/plugin_memory_allocator.hpp
	app/plugin_memory_allocator.cpp
	app/memory_plugin_context_creator.hpp
	app/memory_plugin_context_creator.cpp
)
SOURCE_GROUP( "App" FILES ${APP_CODE_SRCS} )

FILE(GLOB PLUGIN_CONFIG_FILES ${NGT_PLUGIN_CONFIGURATION_FILES})
SOURCE_GROUP( "Plugin Configurations" FILES ${PLUGIN_CONFIG_FILES} )

FILE(GLOB PLUGIN_SETTINGS_FILES ${NGT_PLUGIN_SETTINGS_FILES})
SOURCE_GROUP( "Plugin Settings" FILES ${PLUGIN_SETTINGS_FILES} )

BW_BLOB_SOURCES( BLOB_SRCS
	${APP_CODE_SRCS}
	${PLUGIN_CONFIG_FILES}
	${PLUGIN_SETTINGS_FILES}
)
BW_ADD_TOOL_EXE( generic_app WIN32 ${BLOB_SRCS} )

IF( BW_PLATFORM_WINDOWS )
	SET( PLATFORM_LIBRARIES
		shlwapi)
	SET( PLUGIN_CONFIG_FOLDER plugins )
	SET( PLUGIN_SETTINGS_FOLDER ./ )
ELSEIF( BW_PLATFORM_MAC )
	SET(PLATFORM_LIBRARIES
		core_common)
	SET( PLUGIN_CONFIG_FOLDER ../Resources/plugins )
	SET( PLUGIN_SETTINGS_FOLDER ./ )

	SET( CMAKE_EXE_LINKER_FLAGS
		"${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,@loader_path/../Frameworks"
	)

ENDIF()

BW_TARGET_LINK_LIBRARIES( generic_app PRIVATE
	core_generic_plugin_manager
	wg_memory

	# external libraries
	${PLATFORM_LIBRARIES}
)

BW_CUSTOM_COPY_TO_PROJECT_OUTPUT( ${PLUGIN_CONFIG_FOLDER} "${PLUGIN_CONFIG_FILES}" )
BW_CUSTOM_COPY_TO_PROJECT_OUTPUT( ${PLUGIN_SETTINGS_FOLDER} "${PLUGIN_SETTINGS_FILES}" )

BW_PROJECT_CATEGORY( generic_app "Executables" )
