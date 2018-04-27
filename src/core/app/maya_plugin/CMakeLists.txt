INCLUDE( WGTFMacros )
INCLUDE( WGMacros )
INCLUDE( WGStandardProject )
INCLUDE( WGProjectUserFile )

IF( NOT MAYA_VERSION )
	SET( MAYA_VERSION "2014" )
ENDIF()

WG_FIND_PACKAGE( Maya ${MAYA_VERSION} )

IF( MAYA_THIRD_PARTY_NOT_FOUND )
    MESSAGE( "Maya third party directory is not found, skip building Maya plugin!" )
    RETURN()
ENDIF()

PROJECT( maya_plugin )

# Helper macro for adding maya app
MACRO( BW_ADD_MAYA_APP _PROJNAME _DIRNAME )
	ADD_LIBRARY( ${_PROJNAME} ${ARGN} )

	BW_SET_OUTPUT_NAMES( ${_PROJNAME} )
	BW_SET_BINARY_DIR( ${_PROJNAME} "${WG_TOOLS_BIN_DIR}/${_DIRNAME}/${BW_PLATFORM}" )

	BW_PROJECT_CATEGORY( ${_PROJNAME} "Executables" )
ENDMACRO()


SET( maya_plugin_HEADERS
    app/interfaces/i_application_adapter.hpp
    app/interfaces/i_application_listener.hpp
    app/interfaces/i_window_adapter.hpp
    app/interfaces/i_window_listener.hpp
	app/application_proxy.hpp
	app/qwinhost.h
)

QT4_WRAP_CPP( maya_plugin_MOC_SRCS ${maya_plugin_HEADERS} )

SET( maya_plugin_SRCS
	${maya_plugin_HEADERS}
	${maya_plugin_MOC_SRCS}
	app/maya_plugin.cpp
	app/application_proxy.cpp
	app/qwinhost.cpp
)

SOURCE_GROUP( "Sources" FILES ${maya_plugin_SRCS} )
SOURCE_GROUP( "Generated Sources" FILES ${maya_plugin_MOC_SRCS} )

FILE( GLOB PLUGIN_CONFIG_FILES
	${NGT_PLUGIN_CONFIGURATION_FILES}
	plugins_maya.txt
)
SOURCE_GROUP( "Plugin Configurations" FILES ${PLUGIN_CONFIG_FILES} )
BW_CUSTOM_COPY_TO_PROJECT_OUTPUT( plugins "${PLUGIN_CONFIG_FILES}" )

WG_BLOB_SOURCES( BLOB_SRCS
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
	core_wgtf_app
	core_generic_plugin_manager
	wgtf_memory

	# external libraries
	$(PLATFORM_LIBRARIES)

	${MAYA_Foundation_LIBRARY}
	${MAYA_OpenMaya_LIBRARY}
	${MAYA_OpenMayaUI_LIBRARY}
	${MAYA_OpenMayaAnim_LIBRARY}
	${QT_LIBRARIES}
)

# this is a macro defined in FindMaya that sets up our plugin with standard Maya build settings
MAYA_SET_PLUGIN_PROPERTIES( maya_plugin )

if(MSVC)
	ADD_DEFINITIONS( -DWIN32_LEAN_AND_MEAN )    
endif(MSVC)

