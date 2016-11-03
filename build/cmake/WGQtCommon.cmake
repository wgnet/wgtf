#
# Common directory structure for Qt 5 Projects built with CMake 3.1.1 (or later)
#

CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )

INCLUDE( WGPlatformOptions )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Setup Qt5 Build Paths

IF (NOT Qt5_DIR)
	SET (Qt5_DIR "${WG_TOOLS_SOURCE_DIR}/core/third_party/Qt/${QT_VERSION}" )

	IF ( BW_PLATFORM_WINDOWS )
		# CMAKE_GENERATOR_TOOLSET check is for generating Visual Studio solutions
		# CMAKE_LINKER check is for generating QtCreator projects
		IF ( CMAKE_GENERATOR_TOOLSET STREQUAL "v110_xp"
			OR CMAKE_GENERATOR_TOOLSET STREQUAL "v110"
			OR CMAKE_LINKER MATCHES "Visual Studio 11" )
			SET( Qt5_DIR "${Qt5_DIR}/msvc2012" )
		ELSEIF( CMAKE_GENERATOR_TOOLSET STREQUAL "v120_xp"
			OR CMAKE_GENERATOR_TOOLSET STREQUAL "v120"
			OR CMAKE_LINKER MATCHES "Visual Studio 12")
			SET( Qt5_DIR "${Qt5_DIR}/msvc2013" )
		ELSEIF( CMAKE_GENERATOR_TOOLSET STREQUAL "v140_xp"
			OR CMAKE_GENERATOR_TOOLSET STREQUAL "v140"
			OR CMAKE_LINKER MATCHES "Visual Studio 14")
			SET( Qt5_DIR "${Qt5_DIR}/msvc2015" )
		ELSE()
			MESSAGE( FATAL_ERROR "Cannot find Qt binaries built with the ${CMAKE_GENERATOR_TOOLSET} toolset." )
		ENDIF()
	ELSEIF( BW_PLATFORM_MAC )
		SET( Qt5_DIR "${Qt5_DIR}/clang" )
	ELSE()
		SET( Qt5_DIR "${Qt5_DIR}/___unsupported___" )
	ENDIF()

	IF (BW_ARCH_64)
		SET( Qt5_DIR "${Qt5_DIR}_64" )
	ENDIF()

	IF( NOT EXISTS "${Qt5_DIR}/" AND EXISTS "${Qt5_DIR}_opengl/" )
		SET( Qt5_DIR "${Qt5_DIR}_opengl" )
	ENDIF()
ENDIF()

# Whether this project requires WebEngine deployment
# WebEngine is large in size and should be turned off if not needed
SET( Qt5_WEB_ENGINE_SUPPORT ON )
IF ( CMAKE_GENERATOR_TOOLSET STREQUAL "v110_xp"
	OR CMAKE_GENERATOR_TOOLSET STREQUAL "v110"
	OR CMAKE_LINKER MATCHES "Visual Studio 11" )
	SET( Qt5_WEB_ENGINE_SUPPORT OFF )
ENDIF()

IF( NOT EXISTS "${Qt5_DIR}/" )
	MESSAGE( FATAL_ERROR "Qt build for \"Qt ${QT_VERSION}\" with \"${CMAKE_GENERATOR}\" is not supported, required Qt directory \"${Qt5_DIR}\" does not exists." )
ENDIF()

# Append Qt debug flags to all Qt projects
# Debug and Hybrid, but not Release
SET( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DQT_DECLARATIVE_DEBUG -DQT_QML_DEBUG -DQT_QML_DEBUG_NO_WARNING" )
SET( CMAKE_CXX_FLAGS_HYBRID "${CMAKE_CXX_FLAGS_HYBRID} -DQT_DECLARATIVE_DEBUG -DQT_QML_DEBUG -DQT_QML_DEBUG_NO_WARNING" )

SET( CMAKE_PREFIX_PATH ${Qt5_DIR} CMAKE_PREFIX_PATH )

# Setup Post-buildQt5  paths
SET( Qt5Bin_DIR "${Qt5_DIR}/bin" CACHE TYPE STRING)
SET( Qt5Plugins_DIR "${Qt5_DIR}/plugins" CACHE TYPE STRING)

# Include the required Qt5 Packages
find_package( Qt5Core REQUIRED )
find_package( Qt5Gui REQUIRED )
find_package( Qt5Qml REQUIRED )
find_package( Qt5Quick REQUIRED )
find_package( Qt5QuickWidgets REQUIRED )
find_package( Qt5UiTools REQUIRED )
find_package( Qt5Widgets REQUIRED )
find_package( Qt5Xml REQUIRED )
IF ( Qt5_WEB_ENGINE_SUPPORT )
	find_package( Qt5WebEngine REQUIRED )
ENDIF()

IF(BW_PLATFORM_WINDOWS)
	SET(DEPLOY_QT_COMMAND "${CMAKE_CURRENT_LIST_DIR}/../deployqt.bat")
ELSEIF(BW_PLATFORM_MAC)
	SET(DEPLOY_QT_COMMAND "${CMAKE_CURRENT_LIST_DIR}/../deployqt.sh")
ENDIF()

# Copy Qt dlls to bin.
# Use SET( DEPLOY_QT_SUPPORT ON ) to enable.
# DEPLOY_QT_TARGET_PROJECT must be set. It defines which exe the default is generic_app.
FUNCTION( BW_DEPLOY_QT )
	IF( NOT DEPLOY_QT_SUPPORT )
		MESSAGE( STATUS "Qt deployment disabled" )
		RETURN()
	ENDIF()
	
	SET( RESOURCE_PATH $<TARGET_FILE_DIR:${DEPLOY_QT_TARGET_PROJECT}>/resources )
	SET( QTRESOURCE_PATH $<TARGET_FILE_DIR:${DEPLOY_QT_TARGET_PROJECT}>/qtresources )
	
	IF(BW_PLATFORM_WINDOWS)

		# Deploy everything + webengine
		IF ( Qt5_WEB_ENGINE_SUPPORT )
			SET(WEB_ENGINE_DEPLOY_ARGS)

			# Debug
			ADD_CUSTOM_COMMAND(	TARGET ${PROJECT_NAME} POST_BUILD
				# Removes "Warning: Cannot find Visual Studio installation directory, VCINSTALLDIR is not set."
				COMMAND set VCINSTALLDIR=$(VCInstallDir)
				COMMAND if $<CONFIG:Debug> equ 1 echo "Deploy Qt dependencies, debug"
				COMMAND if $<CONFIG:Debug> equ 1 call "${DEPLOY_QT_COMMAND}" ${Qt5Bin_DIR}
					--debug
					--dir "$<TARGET_FILE_DIR:${DEPLOY_QT_TARGET_PROJECT}>"
					--qmldir "${CMAKE_CURRENT_SOURCE_DIR}" "$<TARGET_FILE:${PROJECT_NAME}>"
					${WEB_ENGINE_DEPLOY_ARGS}
			)

			# Release
			ADD_CUSTOM_COMMAND(	TARGET ${PROJECT_NAME} POST_BUILD
				# Removes "Warning: Cannot find Visual Studio installation directory, VCINSTALLDIR is not set."
				COMMAND set VCINSTALLDIR=$(VCInstallDir)
				COMMAND echo "Deploy Qt dependencies, release webengine"
				# WebEngineView depends on QtWebEngineProcess.exe which depends on release binaries so always deploy them
				COMMAND "${DEPLOY_QT_COMMAND}" ${Qt5Bin_DIR}
					--release
					--dir "$<TARGET_FILE_DIR:${DEPLOY_QT_TARGET_PROJECT}>"
					--qmldir "${CMAKE_CURRENT_SOURCE_DIR}" "$<TARGET_FILE:${PROJECT_NAME}>"
					${WEB_ENGINE_DEPLOY_ARGS}
			)

			# Rename the deployed resources directory to qtresources/resources to avoid issues with engines that use that folder
			# The qt.conf file specifies Data=qtresources so this path can be found
			FILE( GLOB QT_CONF_FILE "${CMAKE_SOURCE_DIR}/../build/qt.conf" )
			GET_FILENAME_COMPONENT( QT_CONF_FILE_NAME "${QT_CONF_FILE}" NAME )
			ADD_CUSTOM_COMMAND(	TARGET ${PROJECT_NAME} POST_BUILD
				# QtWebEngineProcess.exe requires qt.conf to point to qtresources
				COMMAND echo "Deploy qt.conf"
				COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_CONF_FILE} 
					$<TARGET_FILE_DIR:${DEPLOY_QT_TARGET_PROJECT}>/${QT_CONF_FILE_NAME} )

			ADD_CUSTOM_COMMAND(	TARGET ${PROJECT_NAME} POST_BUILD
				# Requires qt.conf to point to qtresources
				COMMAND echo "Move resources to qtresources"
				COMMAND ${CMAKE_COMMAND} -E remove_directory \"${QTRESOURCE_PATH}\"
				COMMAND ${CMAKE_COMMAND} -E make_directory \"${QTRESOURCE_PATH}\"
				COMMAND ${CMAKE_COMMAND} -E rename \"${RESOURCE_PATH}\" \"${QTRESOURCE_PATH}/resources\" )

		# Deploy everything, except webengine
		ELSE( Qt5_WEB_ENGINE_SUPPORT )
			SET(WEB_ENGINE_DEPLOY_ARGS --no-webengine --no-webenginecore --no-webenginewidgets)

			# Debug + Release
			ADD_CUSTOM_COMMAND(	TARGET ${PROJECT_NAME} POST_BUILD
				# Removes "Warning: Cannot find Visual Studio installation directory, VCINSTALLDIR is not set."
				COMMAND set VCINSTALLDIR=$(VCInstallDir)
				COMMAND echo "Deploy Qt dependencies, excluding webengine"
				COMMAND "${DEPLOY_QT_COMMAND}" ${Qt5Bin_DIR}
					$<$<CONFIG:Debug>:--debug> $<$<NOT:$<CONFIG:Debug>>:--release>
					--dir "$<TARGET_FILE_DIR:${DEPLOY_QT_TARGET_PROJECT}>"
					--qmldir "${CMAKE_CURRENT_SOURCE_DIR}" "$<TARGET_FILE:${PROJECT_NAME}>"
					${WEB_ENGINE_DEPLOY_ARGS}
			)
		ENDIF( Qt5_WEB_ENGINE_SUPPORT )

	ELSEIF(BW_PLATFORM_MAC)
		ADD_CUSTOM_COMMAND(	TARGET ${PROJECT_NAME} POST_BUILD
			COMMAND "${DEPLOY_QT_COMMAND}" ${Qt5Bin_DIR} $<TARGET_FILE_DIR:${DEPLOY_QT_TARGET_PROJECT}>/../..
				-always-overwrite
				-qmldir="${CMAKE_CURRENT_SOURCE_DIR}"
		)
	ELSE()
		MESSAGE( FATAL_ERROR "BW_DEPLOY_QT(): Unsupported platform!" )
	ENDIF()
ENDFUNCTION()

MACRO(BW_GLOB_RESOURCE_FILES)
	FILE(GLOB_RECURSE QML_SRCS *.qml qmldir)
	SOURCE_GROUP("QML Files" FILES ${QML_SRCS})

	FILE(GLOB_RECURSE JS_SRCS *.js)
	SOURCE_GROUP("JS Files" FILES ${JS_SRCS})

	FILE(GLOB_RECURSE UI_SRCS *.ui)
	SOURCE_GROUP("UI Files" FILES ${UI_SRCS})

	FILE(GLOB_RECURSE ICON_SRCS *.png *.bmp)
	SOURCE_GROUP("Icon Files" FILES ${ICON_SRCS})

	SET( ALL_SRCS ${ALL_SRCS} ${QML_SRCS} ${JS_SRCS} ${UI_SRCS})
ENDMACRO()

IF( BW_PLATFORM_WINDOWS )
	SET( QRC_GENERATOR ${CMAKE_CURRENT_LIST_DIR}/../qrc_generator.bat )
ELSE()
	SET( QRC_GENERATOR ${CMAKE_CURRENT_LIST_DIR}/../qrc_generator.sh )
ENDIF()

MACRO(BW_QT_ADD_QRC _PREFIX _DIR)
	IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_DIR}" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${_DIR}")
		SET(_RESOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${_DIR}")
	ELSEIF(EXISTS "${_DIR}" AND IS_DIRECTORY "${_DIR}")
		SET(_RESOURCE_DIR ${_DIR})
	ENDIF()

	EXECUTE_PROCESS(
		COMMAND ${QRC_GENERATOR} ${PROJECT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${_PREFIX}.qrc ${_PREFIX} ${_RESOURCE_DIR}
	)
	QT5_ADD_RESOURCES(COMPILED_RESOURCES ${CMAKE_CURRENT_BINARY_DIR}/${_PREFIX}.qrc)
	SET(RESOURCE_FILES ${RESOURCE_FILES} ${CMAKE_CURRENT_BINARY_DIR}/${_PREFIX}.qrc)
	SOURCE_GROUP( "Resource Files" FILES ${RESOURCE_FILES} )
	SOURCE_GROUP( "Compiled Resources" FILES ${COMPILED_RESOURCES} )
ENDMACRO()

MACRO(BW_QT_ADD_CONFIG_QRC)
	IF ( BW_PLATFORM_WINDOWS )
		SET(QT_CONF_FILE "${CMAKE_SOURCE_DIR}/../build/qt.conf.qrc")
		QT5_ADD_RESOURCES(COMPILED_RESOURCES ${QT_CONF_FILE})
		SET(RESOURCE_FILES ${RESOURCE_FILES} ${QT_CONF_FILE})
		SOURCE_GROUP( "Resource Files" FILES ${RESOURCE_FILES} )
		SOURCE_GROUP( "Compiled Resources" FILES ${COMPILED_RESOURCES} )
	ENDIF()
ENDMACRO()
