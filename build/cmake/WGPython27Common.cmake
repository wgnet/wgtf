
IF( BW_PLATFORM_WINDOWS )
	SET( DEPLOY_PYTHON_COMMAND "./deploy_python.bat" )
ENDIF()

BW_GET_EXECUTABLE_DST_DIR( EXE_DIR )

# Copy Python pycs to bin/generic_app_test/win64.
# Use SET( DEPLOY_PYTHON_SUPPORT ON ) to enable.
# _TARGET_PROJECT name of the source project.
#	E.g. plg_example.
# _SCRIPT_DIR relative path to script *.py sources from the plugin.
#	E.g. ${CMAKE_CURRENT_LIST_DIR}/scripts.
# _BIN_SUBDIR_NAME name of the destination folder in bin.
#	E.g. scripts/plg_example.
FUNCTION( WG_DEPLOY_PYTHON _TARGET_PROJECT _SCRIPT_DIR _BIN_SUBDIR_NAME )
	IF( NOT DEPLOY_PYTHON_SUPPORT )
		MESSAGE( STATUS "Python deployment disabled" )
		RETURN()
	ENDIF()

	MESSAGE( STATUS "${_TARGET_PROJECT} deploying Python from ${_SCRIPT_DIR} to ${EXE_DIR}/${_BIN_SUBDIR_NAME}" )
	IF( BW_PLATFORM_WINDOWS )
		ADD_CUSTOM_COMMAND(	TARGET ${_TARGET_PROJECT} POST_BUILD
			COMMAND "${DEPLOY_PYTHON_COMMAND}" ${_SCRIPT_DIR} ${EXE_DIR}/${_BIN_SUBDIR_NAME}
			WORKING_DIRECTORY ${CMAKE_MODULE_PATH}/..
		)
	ELSEIF( BW_PLATFORM_MAC )
		MESSAGE( STATUS "WG_DEPLOY_PYTHON(): TODO not implemented for Mac" )
	ELSE()
		MESSAGE( FATAL_ERROR "WG_DEPLOY_PYTHON(): Unsupported platform!" )
	ENDIF()
ENDFUNCTION()

