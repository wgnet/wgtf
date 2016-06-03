INCLUDE( WGMacros )
INCLUDE( WGMayaProject )
INCLUDE( WGProjectUserFile )

IF( MAYA_THIRD_PARTY_NOT_FOUND )
    MESSAGE( "Maya third party directory is not found, skip building Maya plugin!" )
    RETURN()
ENDIF()

PROJECT( maya_plugin )

# Helper macro for adding maya app
MACRO( BW_ADD_MAYA_APP _PROJNAME _DIRNAME )
	ADD_LIBRARY( ${_PROJNAME} ${ARGN} )

	SET_TARGET_PROPERTIES( ${_PROJNAME} PROPERTIES
		DEBUG_OUTPUT_NAME
		"${_PROJNAME}_d" )
	SET_TARGET_PROPERTIES( ${_PROJNAME} PROPERTIES
		HYBRID_OUTPUT_NAME
		"${_PROJNAME}" )

	BW_SET_BINARY_DIR( ${_PROJNAME} "${WG_TOOLS_BIN_DIR}/${_DIRNAME}/${BW_PLATFORM}" )

	BW_PROJECT_CATEGORY( ${_PROJNAME} "Executables" )
ENDMACRO()


SET( maya_plugin_HEADERS
    app/interfaces/i_application_adapter.hpp
    app/interfaces/i_application_listener.hpp
    app/interfaces/i_window_adapter.hpp
    app/interfaces/i_window_listener.hpp
	app/ngt_application_proxy.hpp
	app/qwinhost.h
)

QT4_WRAP_CPP( maya_plugin_MOC_SRCS ${maya_plugin_HEADERS} )

SET( maya_plugin_SRCS
	${maya_plugin_HEADERS}
	${maya_plugin_MOC_SRCS}
	app/maya_plugin.cpp
	app/ngt_application_proxy.cpp
	app/qwinhost.cpp
	../generic_app/app/plugin_memory_allocator.hpp
	../generic_app/app/plugin_memory_allocator.cpp
	../generic_app/app/memory_plugin_context_creator.hpp
	../generic_app/app/memory_plugin_context_creator.cpp
)

SOURCE_GROUP( "Sources" FILES ${maya_plugin_SRCS} )
SOURCE_GROUP( "Generated Sources" FILES ${maya_plugin_MOC_SRCS} )

FILE( GLOB PLUGIN_CONFIG_FILES
	${NGT_PLUGIN_CONFIGURATION_FILES}
	plugins_maya.txt
)
SOURCE_GROUP( "Plugin Configurations" FILES ${PLUGIN_CONFIG_FILES} )
BW_CUSTOM_COPY_TO_PROJECT_OUTPUT( plugins "${PLUGIN_CONFIG_FILES}" )

BW_BLOB_SOURCES( BLOB_SRCS
	${maya_plugin_SRCS}
	${PLUGIN_CONFIG_FILES}
)

BW_ADD_MAYA_APP( maya_plugin ${TARGET_PLUGIN_APP_FOLDER_NAME} SHARED ${BLOB_SRCS} )

IF(BW_PLATFORM_WINDOWS)
	SET(PLATFORM_LIBRARIES
		shlwapi)
ELSE()
	SET(PLATFORM_LIBRARIES)
ENDIF()

BW_TARGET_LINK_LIBRARIES( maya_plugin PRIVATE
	core_common
	core_generic_plugin_manager
	wg_memory

	# external libraries
	$(PLATFORM_LIBRARIES)

	$<$<NOT:$<CONFIG:Debug>>:${MAYA_Foundation_LIBRARY}>
	$<$<NOT:$<CONFIG:Debug>>:${MAYA_OpenMaya_LIBRARY}>
	$<$<NOT:$<CONFIG:Debug>>:${MAYA_OpenMayaUI_LIBRARY}>
	$<$<NOT:$<CONFIG:Debug>>:${MAYA_OpenMayaAnim_LIBRARY}>
	${QT_LIBRARIES}
)

# this is a macro defined in FindMaya that sets up our plugin with standard Maya build settings
MAYA_SET_PLUGIN_PROPERTIES( maya_plugin )

SET_TARGET_PROPERTIES(maya_plugin PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD_DEBUG True)
