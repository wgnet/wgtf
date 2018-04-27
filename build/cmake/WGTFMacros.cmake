INCLUDE_DIRECTORIES( ${WG_TOOLS_SOURCE_DIR} )

INCLUDE( WGPlatformOptions )
INCLUDE( DirMacros )

# Adds Consumer_Release build config
SET(BW_BUILD_CMAKE_DIR ${CMAKE_CURRENT_LIST_DIR})

MACRO( WG_FIND_PACKAGE )
    IF ( ALL_THIRD_PARTY_REQUIRED )
        FIND_PACKAGE( ${ARGV} REQUIRED )
    ELSE()
        FIND_PACKAGE( ${ARGV} )
    ENDIF()
ENDMACRO()

FUNCTION( WG_AUTO_SOURCE_GROUPS )
    SET(_GROUPS)
 
    FOREACH(_FILENAME ${ARGN})
        GET_FILENAME_COMPONENT(_DIRNAME ${_FILENAME} DIRECTORY)
        STRING(REPLACE "/" "_" _GROUP_NAME "${_DIRNAME}")
        SET(_GROUP_NAME "GROUP_${_GROUP_NAME}")
 
        IF (${_GROUP_NAME})
            LIST(APPEND ${_GROUP_NAME} ${_FILENAME})
        ELSE()
            SET(${_GROUP_NAME} ${_FILENAME})
            LIST(APPEND _GROUPS ${_GROUP_NAME})
        ENDIF()
    ENDFOREACH()
 
    FOREACH (_GROUP_NAME ${_GROUPS})
        STRING(REGEX REPLACE "^GROUP_" "" _GROUP_LABEL "${_GROUP_NAME}")
        STRING(REPLACE "_" "\\" _GROUP_LABEL "${_GROUP_LABEL}")
        SOURCE_GROUP("${_GROUP_LABEL}" FILES ${${_GROUP_NAME}})
    ENDFOREACH()
ENDFUNCTION()

FUNCTION( BW_REMOVE_COMPILE_FLAGS )
	SET( _CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" )
	SET( _CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" )
	FOREACH( _FLAG IN LISTS ARGN )
		STRING( REGEX REPLACE "${_FLAG}" "" _CMAKE_C_FLAGS "${_CMAKE_C_FLAGS}"  )
		STRING( REGEX REPLACE "${_FLAG}" "" _CMAKE_CXX_FLAGS "${_CMAKE_CXX_FLAGS}" )
	ENDFOREACH()
	SET( CMAKE_C_FLAGS "${_CMAKE_C_FLAGS}" PARENT_SCOPE )
	SET( CMAKE_CXX_FLAGS "${_CMAKE_CXX_FLAGS}" PARENT_SCOPE )
ENDFUNCTION()


# Standard BW precompiled headers
MACRO( LESTA_PRECOMPILED_HEADER _PROJNAME _PCHNAME )
	BW_APPEND_TARGET_PROPERTIES( ${_PROJNAME} COMPILE_FLAGS "/Yu${_PCHNAME}" )
	SET_SOURCE_FILES_PROPERTIES( pch.cpp PROPERTIES COMPILE_FLAGS "/Yc${_PCHNAME}" )
ENDMACRO( LESTA_PRECOMPILED_HEADER )

# For appending properties because SET_TARGET_PROPERTIES overwrites
FUNCTION( BW_APPEND_TARGET_PROPERTIES _PROJNAME _PROPNAME _PROPS_TO_APPEND )
	SET( NEW_PROPS ${_PROPS_TO_APPEND} )
	GET_TARGET_PROPERTY( EXISTING_PROPS ${_PROJNAME} ${_PROPNAME} )
	IF( EXISTING_PROPS )
		SET( NEW_PROPS "${NEW_PROPS} ${EXISTING_PROPS}" )
	ENDIF()
	SET_TARGET_PROPERTIES( ${_PROJNAME} PROPERTIES ${_PROPNAME} ${NEW_PROPS} )
ENDFUNCTION()

FUNCTION( WGTF_SETUP_COMPILE_FLAGS _TARGET)
	
	# Calculate the set of compile flags that need to be applied to WGTF projects
	IF( MSVC )
		SET( WGTF_CXX_FLAGS "/EHsc /wd4251 /wd4548 /W3 /GR /DQT_QML_DEBUG /D_HAS_EXCEPTIONS=1")
	ENDIF()

	IF( APPLE )
		SET( WGTF_CXX_FLAGS "${WGTF_CXX_FLAGS} -std=c++1y -stdlib=libc++" )
	ENDIF()

	BW_APPEND_TARGET_PROPERTIES(${_TARGET} COMPILE_FLAGS ${WGTF_CXX_FLAGS})
	
ENDFUNCTION()

FUNCTION( BW_APPEND_TARGET_LIST_PROPERTIES _PROJNAME _PROPNAME )
	SET( NEW_PROPS ${ARGN} )

	GET_TARGET_PROPERTY( EXISTING_PROPS ${_PROJNAME} ${_PROPNAME} )
	IF( EXISTING_PROPS )
		LIST( APPEND NEW_PROPS ${EXISTING_PROPS} )
	ENDIF()

	SET_TARGET_PROPERTIES( ${_PROJNAME} PROPERTIES ${_PROPNAME} "${NEW_PROPS}" )
ENDFUNCTION()

# For appending to LINK_FLAGS because SET_TARGET_PROPERTIES overwrites it
FUNCTION( BW_APPEND_LINK_FLAGS _PROJNAME _FLAGS_TO_APPEND )
	BW_APPEND_TARGET_PROPERTIES( ${_PROJNAME} LINK_FLAGS ${_FLAGS_TO_APPEND} )
ENDFUNCTION()

# Add Microsoft Windows Common-Controls as a dependency on the target
MACRO( BW_USE_MICROSOFT_COMMON_CONTROLS _PROJNAME )
	BW_APPEND_LINK_FLAGS( ${_PROJNAME}
		"/MANIFESTDEPENDENCY:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' publicKeyToken='6595b64144ccf1df' language='*' processorArchitecture='*'\""
	)
ENDMACRO( BW_USE_MICROSOFT_COMMON_CONTROLS )


# Assigns the given target to a Solution Folder
# http://athile.net/library/blog/?p=288
MACRO( BW_PROJECT_CATEGORY _PROJNAME _CATEGORY )
	SET_PROPERTY( TARGET ${_PROJNAME} PROPERTY FOLDER ${_CATEGORY} )
ENDMACRO( BW_PROJECT_CATEGORY )


# Links the given executable target name to all BW_LIBRARY_PROJECTS
MACRO( BW_LINK_LIBRARY_PROJECTS _PROJNAME )
	MESSAGE( WARNING
		 "BW_LINK_LIBRARY_PROJECTS is deprecated. Use BW_LINK_TARGET_LIBRARIES instead." )
	ARRAY2D_BEGIN_LOOP( _islooping "${BW_LIBRARY_PROJECTS}" 2 "libname;libpath" )
		WHILE( _islooping )
			TARGET_LINK_LIBRARIES( ${_PROJNAME} ${libname} )
			ARRAY2D_ADVANCE()
		ENDWHILE()
	ARRAY2D_END_LOOP()
ENDMACRO( BW_LINK_LIBRARY_PROJECTS )

# Set the output directory for the given executable target name to
# the given location.
FUNCTION( BW_SET_BINARY_DIR _PROJNAME _DIRNAME )
	FOREACH(CONFIGURATION ${CMAKE_CONFIGURATION_TYPES})
		string( TOUPPER ${CONFIGURATION} CONFIGURATION )
		SET_TARGET_PROPERTIES( ${_PROJNAME} PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY_${CONFIGURATION} "${_DIRNAME}"
			PDB_OUTPUT_DIRECTORY_${CONFIGURATION} "${_DIRNAME}")
		IF (BW_PLATFORM_MAC)
			SET_TARGET_PROPERTIES( ${_PROJNAME} PROPERTIES
				LIBRARY_OUTPUT_DIRECTORY_${CONFIGURATION} "${_DIRNAME}"
				PDB_OUTPUT_DIRECTORY_${CONFIGURATION} "${_DIRNAME}")
		ENDIF()
	ENDFOREACH()
ENDFUNCTION( BW_SET_BINARY_DIR )

# Setup the output directories for the given Python extension target name
FUNCTION( BW_SET_PYTHON_EXTENSION_OUTPUT_DIR _PROJNAME _DIRNAME _PLATFORM )
	# See Python27ScriptingEngine::init() in core_python27/scripting_engine.cpp
	BW_GET_EXECUTABLE_DST_DIR( EXE_DIR )
	SET( MODULE_ROOT ${EXE_DIR} )
	IF( WIN32 )
		# On WIN32, debug modules have a "_d" suffix
		SET_TARGET_PROPERTIES( ${_PROJNAME} PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY_DEBUG "${MODULE_ROOT}"
			ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${MODULE_ROOT}"
			LIBRARY_OUTPUT_DIRECTORY_DEBUG "${MODULE_ROOT}"
			)
		# On WIN32, hybrid modules have a "_h" suffix
		SET_TARGET_PROPERTIES( ${_PROJNAME} PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY_HYBRID "${MODULE_ROOT}"
			ARCHIVE_OUTPUT_DIRECTORY_HYBRID "${MODULE_ROOT}"
			LIBRARY_OUTPUT_DIRECTORY_HYBRID "${MODULE_ROOT}"
			)
		# On WIN32, release modules have a "" suffix
		SET_TARGET_PROPERTIES( ${_PROJNAME} PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY_RELEASE "${MODULE_ROOT}"
			ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${MODULE_ROOT}"
			LIBRARY_OUTPUT_DIRECTORY_RELEASE "${MODULE_ROOT}"
			)
	ELSE()
		SET_TARGET_PROPERTIES( ${_PROJNAME} PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY_DEBUG "${MODULE_ROOT}_debug"
			ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${MODULE_ROOT}_debug"
			LIBRARY_OUTPUT_DIRECTORY_DEBUG "${MODULE_ROOT}_debug"
			)
		SET_TARGET_PROPERTIES( ${_PROJNAME} PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY_HYBRID "${MODULE_ROOT}"
			ARCHIVE_OUTPUT_DIRECTORY_HYBRID "${MODULE_ROOT}"
			LIBRARY_OUTPUT_DIRECTORY_HYBRID "${MODULE_ROOT}"
			)
		SET_TARGET_PROPERTIES( ${_PROJNAME} PROPERTIES
			RUNTIME_OUTPUT_DIRECTORY_RELEASE "${MODULE_ROOT}_release"
			ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${MODULE_ROOT}_release"
			LIBRARY_OUTPUT_DIRECTORY_RELEASE "${MODULE_ROOT}_release"
			)
	ENDIF()
ENDFUNCTION( BW_SET_PYTHON_EXTENSION_OUTPUT_DIR _PROJNAME )

MACRO( BW_SET_OUTPUT_NAMES _PROJNAME )
	BW_GET_DEBUG_POSTFIX( DEBUG_POSTFIX )
	BW_GET_HYBRID_POSTFIX( HYBRID_POSTFIX )
	BW_GET_RELEASE_POSTFIX( RELEASE_POSTFIX )
	
	SET_TARGET_PROPERTIES( ${_PROJNAME} PROPERTIES
		DEBUG_OUTPUT_NAME
		"${_PROJNAME}${DEBUG_POSTFIX}" )
	SET_TARGET_PROPERTIES( ${_PROJNAME} PROPERTIES
		HYBRID_OUTPUT_NAME
		"${_PROJNAME}${HYBRID_POSTFIX}" )
	SET_TARGET_PROPERTIES( ${_PROJNAME} PROPERTIES
		RELEASE_OUTPUT_NAME
		"${_PROJNAME}${RELEASE_POSTFIX}" )
ENDMACRO( BW_SET_OUTPUT_NAMES )

# Marks the executable given as a Unit test
MACRO( BW_ADD_TEST _PROJNAME )
	BW_GET_TEST_DST_DIR(TEST_DIR)
	ADD_TEST( NAME ${_PROJNAME}
		COMMAND $<TARGET_FILE:${_PROJNAME}>
		WORKING_DIRECTORY ${TEST_DIR})

	BW_SET_OUTPUT_NAMES( ${_PROJNAME} )
	BW_SET_BINARY_DIR( ${_PROJNAME} "${TEST_DIR}" )

	BW_DEPLOY_DEPENDENCY_TARGETS( ${_PROJNAME}  )
	WGTF_SETUP_COMPILE_FLAGS(${_PROJNAME})
ENDMACRO( BW_ADD_TEST )

MACRO( BW_ADD_TOOL_TEST _PROJNAME )
	BW_ADD_TEST( ${_PROJNAME} )
ENDMACRO( BW_ADD_TOOL_TEST )

MACRO( BW_TARGET_LINK_LIBRARIES )
	TARGET_LINK_LIBRARIES(${ARGN} core_common)
ENDMACRO( BW_TARGET_LINK_LIBRARIES )

# Add a library
FUNCTION( BW_ADD_LIBRARY _PROJNAME )
	ADD_LIBRARY( ${_PROJNAME} ${ARGN} )
	WGTF_SETUP_COMPILE_FLAGS(${_PROJNAME})

	GET_TARGET_PROPERTY( LIB_TYPE ${_PROJNAME} TYPE )

	IF( "${LIB_TYPE}" STREQUAL "SHARED_LIBRARY" OR
		"${LIB_TYPE}" STREQUAL "MODULE_LIBRARY"
	)

		BW_SET_OUTPUT_NAMES( ${_PROJNAME} )
		BW_GET_EXECUTABLE_DST_DIR( EXE_DST )
		BW_SET_BINARY_DIR( ${_PROJNAME} "${EXE_DST}" )
	ENDIF()
ENDFUNCTION( BW_ADD_LIBRARY )

# Add an executable
MACRO( BW_ADD_EXECUTABLE _PROJNAME )
	ADD_EXECUTABLE( ${_PROJNAME} ${ARGN} )
	WGTF_SETUP_COMPILE_FLAGS(${_PROJNAME})
ENDMACRO( BW_ADD_EXECUTABLE )

# Helper macro for adding a tool executable
MACRO( BW_ADD_TOOL_EXE _PROJNAME )
	IF (BW_PLATFORM_MAC)
		IF (NOT MACOSX_BUNDLE_ICON_FILE)
			SET( MACOSX_BUNDLE_ICON_FILE application.icns )
			SET( _ICON ${BW_BUILD_CMAKE_DIR}/xcode/application.icns )
			SET_SOURCE_FILES_PROPERTIES(${_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
		ENDIF()
		BW_ADD_EXECUTABLE( ${_PROJNAME} MACOSX_BUNDLE ${ARGN} ${_ICON} )
	ELSE (BW_PLATFORM_WINDOWS)
		BW_ADD_EXECUTABLE( ${_PROJNAME} WIN32 ${ARGN} )
	ENDIF()

	BW_SET_OUTPUT_NAMES( ${_PROJNAME} )
	BW_GET_EXECUTABLE_DST_DIR(EXE_DIR)
	BW_SET_BINARY_DIR( ${_PROJNAME} "${EXE_DIR}" )

	IF( BW_PLATFORM_WINDOWS AND CMAKE_MFC_FLAG EQUAL 2 )
		IF( NOT BW_NO_UNICODE )
			# Force entry point for MFC
			BW_APPEND_LINK_FLAGS( ${_PROJNAME} "/entry:wWinMainCRTStartup" )
		ENDIF()
		BW_USE_MICROSOFT_COMMON_CONTROLS( ${_PROJNAME} )
	ENDIF()

	BW_PROJECT_CATEGORY( ${_PROJNAME} "Executables" )
ENDMACRO()

# Helper macro for adding a tool plugin
MACRO( BW_ADD_TOOL_PLUGIN _PROJNAME)
	ADD_LIBRARY( ${_PROJNAME} ${ARGN} )
	WGTF_SETUP_COMPILE_FLAGS(${_PROJNAME})

	BW_SET_OUTPUT_NAMES( ${_PROJNAME} )
	BW_GET_EXECUTABLE_DST_DIR(EXE_DIR)
	BW_SET_BINARY_DIR( ${_PROJNAME} "${EXE_DIR}/plugins" )

	BW_PROJECT_CATEGORY( ${_PROJNAME} "Plugins" )

	IF (BW_PLATFORM_MAC)
		BW_COPY_PLUGIN_TO_BUNDLE( ${_PROJNAME})
	ENDIF()
ENDMACRO()

# Helper macro for adding a tool plugin
MACRO( BW_ADD_TOOL_TEST_PLUGIN _PROJNAME )
	ADD_LIBRARY( ${_PROJNAME} ${ARGN} )
	WGTF_SETUP_COMPILE_FLAGS(${_PROJNAME})

	BW_SET_OUTPUT_NAMES( ${_PROJNAME} )
	BW_GET_TEST_DST_DIR(TEST_DIR)
	BW_SET_BINARY_DIR( ${_PROJNAME} "${TEST_DIR}/plugins" )

	BW_PROJECT_CATEGORY( ${_PROJNAME} "Unit Tests/Plugins" )
ENDMACRO()

# Helper macro for adding asset pipeline exes and dlls
MACRO( BW_ADD_ASSETPIPELINE_EXE _PROJNAME )
	BW_ADD_TOOL_EXE( ${_PROJNAME} asset_pipeline ${ARGN} )
ENDMACRO()

MACRO( BW_ADD_ASSETPIPELINE_DLL _PROJNAME )
	BW_ADD_LIBRARY( ${_PROJNAME} SHARED ${ARGN} )

	BW_GET_ASSET_PIPELINE_DST_DIR(ASSET_DIR)
	BW_SET_BINARY_DIR( ${_PROJNAME} "${ASSET_DIR}" )
	BW_PROJECT_CATEGORY( ${_PROJNAME} "Asset Pipeline/Converters" )
ENDMACRO()

MACRO( BW_ADD_UNITTEST_ASSETPIPELINE_DLL _PROJNAME )
	BW_ADD_LIBRARY( ${_PROJNAME} SHARED ${ARGN} )

	BW_GET_TEST_DST_DIR(TEST_DIR)
	BW_SET_BINARY_DIR( ${_PROJNAME} "${TEST_DIR}" )
	BW_PROJECT_CATEGORY( ${_PROJNAME} "Unit Tests/Asset Pipeline" )
ENDMACRO()

MACRO( BW_CUSTOM_COMMAND _PROJNAME _CMD )
	ADD_CUSTOM_TARGET( ${_PROJNAME}
		COMMAND ${_CMD} ${ARGN}
	)
	BW_PROJECT_CATEGORY( ${_PROJNAME} "Build Commands" )
ENDMACRO( BW_CUSTOM_COMMAND )

# Copy output from one target to output dir of another target
# **Important Note**
# http://www.cmake.org/cmake/help/v2.8.8/cmake.html#command:add_custom_command
# "Do not list the output in more than one independent target that may build in
# parallel or the two instances of the rule may conflict
# (instead use add_custom_target to drive the command and make the other
# targets depend on that one)."
FUNCTION( BW_COPY_TARGET _TO_TARGET _FROM_TARGET )
	ADD_CUSTOM_COMMAND(
		TARGET ${_TO_TARGET}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE:${_FROM_TARGET}>
			$<TARGET_FILE_DIR:${_TO_TARGET}>/$<TARGET_FILE_NAME:${_FROM_TARGET}>
		COMMENT "Copying from ${_FROM_TARGET} to ${_TO_TARGET}'s output dir." )
ENDFUNCTION()
FUNCTION( BW_COPY_TARGET_PDB _TO_TARGET _FROM_TARGET )
	ADD_CUSTOM_COMMAND(
		TARGET ${_TO_TARGET}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_PDB_FILE:${_FROM_TARGET}>
			$<TARGET_FILE_DIR:${_TO_TARGET}>/$<TARGET_PDB_FILE_NAME:${_FROM_TARGET}>
		COMMENT "Copying from ${_FROM_TARGET} to ${_TO_TARGET}'s output dir." )
ENDFUNCTION()

# Copy binary from output directory to the unit test output directory
# **Important Note**
# http://www.cmake.org/cmake/help/v2.8.8/cmake.html#command:add_custom_command
# "Do not list the output in more than one independent target that may build in
# parallel or the two instances of the rule may conflict
# (instead use add_custom_target to drive the command and make the other
# targets depend on that one)."
FUNCTION( BW_COPY_TARGET_TO_UNIT_TEST_BIN _FROM_TARGET )
	BW_GET_TEST_DST_DIR(TEST_DIR)
	ADD_CUSTOM_COMMAND(
		TARGET ${_FROM_TARGET}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE:${_FROM_TARGET}>
			${TEST_DIR}/$<TARGET_FILE_NAME:${_FROM_TARGET}>
		COMMENT "Copying from ${_FROM_TARGET} to ${TEST_DIR}."
		VERBATIM
	)
ENDFUNCTION()

# Copy plugin from output directory to the unit test plugins directory
# **Important Note**
# http://www.cmake.org/cmake/help/v2.8.8/cmake.html#command:add_custom_command
# "Do not list the output in more than one independent target that may build in
# parallel or the two instances of the rule may conflict
# (instead use add_custom_target to drive the command and make the other
# targets depend on that one)."
FUNCTION( BW_COPY_PLUGIN_TO_UNIT_TEST_BIN _FROM_TARGET )
	BW_GET_TEST_DST_DIR(TEST_DIR)
	ADD_CUSTOM_COMMAND(
		TARGET ${_FROM_TARGET}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE:${_FROM_TARGET}>
			${TEST_DIR}/plugins/$<TARGET_FILE_NAME:${_FROM_TARGET}>
		COMMENT "Copying from ${_FROM_TARGET} to ${TEST_DIR}/plugins."
		VERBATIM
	)
ENDFUNCTION()

FUNCTION( BW_COPY_PLUGIN_TO_BUNDLE _FROM_TARGET)
	BW_GET_EXECUTABLE_DST_DIR(EXE_DIR)
	BW_GET_DEBUG_POSTFIX( DEBUG_POSTFIX )
	BW_GET_HYBRID_POSTFIX( HYBRID_POSTFIX )
	BW_GET_RELEASE_POSTFIX( RELEASE_POSTFIX )
	ADD_CUSTOM_COMMAND(
		TARGET ${_FROM_TARGET}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE:${_FROM_TARGET}>
			${EXE_DIR}/${BW_BUNDLE_NAME}$<$<CONFIG:Debug>:${DEBUG_POSTFIX}>$<$<CONFIG:Hybrid>:${HYBRID_POSTFIX}>$<$<CONFIG:Release>:${RELEASE_POSTFIX}>.app/Contents/PlugIns/plugins/$<TARGET_FILE_NAME:${_FROM_TARGET}>
		COMMENT "Copying from ${_FROM_TARGET} to ${EXE_DIR}/${BW_BUNDLE_NAME}*.app/Contents/PlugIns/plugins."
		VERBATIM
	)
ENDFUNCTION()

FUNCTION( BW_SET_OPTIONAL_FILES _RETURN_VAR )
	FOREACH( _FILENAME ${ARGN} )
		IF( EXISTS ${_FILENAME} )
			LIST( APPEND _DETECTED_FILES ${_FILENAME} )
		ENDIF()
	ENDFOREACH()
	SET( ${_RETURN_VAR} ${_DETECTED_FILES} PARENT_SCOPE )
ENDFUNCTION()

#-------------------------------------------------------------------

#
# 2D array helpers: http://www.cmake.org/pipermail/cmake/2011-November/047829.html
#-------------------------------------------------------------------

macro( array2d_get_item out_value offset )
    math( EXPR _finalindex "${_array2d_index}+${offset}" )
    list( GET _array2d_array ${_finalindex} _item )
    set( ${out_value} "${_item}" )
endmacro()

#-------------------------------------------------------------------

macro( array2d_begin_loop out_advanced array width var_names )
    set( _array2d_out_advanced ${out_advanced} )
    set( _array2d_index 0 )
    set( _array2d_array ${array} )
    set( _array2d_width ${width} )
    set( _array2d_var_names ${var_names} )
    array2d_advance()
endmacro()

#-------------------------------------------------------------------

macro( array2d_advance )
    if( NOT _array2d_array )
        set( ${_array2d_out_advanced} false )
    else()
        list( LENGTH _array2d_array _size )
        math( EXPR _remaining "${_size}-${_array2d_index}" )

        if( (_array2d_width LESS 1) OR (_size LESS _array2d_width) OR (_remaining LESS _array2d_width) )
            set( ${_array2d_out_advanced} false )
        else()
            math( EXPR _adjusted_width "${_array2d_width}-1" )
            foreach( offset RANGE ${_adjusted_width} )
                list( GET _array2d_var_names ${offset} _var_name )
                array2d_get_item( ${_var_name} ${offset} )
            endforeach()

            math( EXPR _index "${_array2d_index}+${_array2d_width}" )
            set( _array2d_index ${_index} )
            set( ${_array2d_out_advanced} true )
        endif()
    endif()
endmacro()

#-------------------------------------------------------------------

macro( array2d_end_loop )
endmacro()
#-------------------------------------------------------------------



# Old way of iterating through 2D list, kept for reference.
#LIST( LENGTH BW_LIBRARY_PROJECTS _itercount )
#MATH( EXPR _itercount "(${_itercount} / 2)-1" )
#FOREACH( i RANGE( ${_itercount} ) )
#	MATH( EXPR i1 "${i}*2" )
#	MATH( EXPR i2 "${i}*2+1" )
#	LIST( GET BW_LIBRARY_PROJECTS ${i1} libname )
#	LIST( GET BW_LIBRARY_PROJECTS ${i2} libpath )

#	MESSAGE( STATUS "Adding library: ${libname} from ${libpath}" )
#	ADD_SUBDIRECTORY( ${libpath} )
#ENDFOREACH()

# Copy any plugin resource files to a folder named after the plugin
MACRO( BW_COPY_PLUGIN_RESOURCES _RESOURCES )
	SOURCE_GROUP( "Resource Files" FILES ${_RESOURCES} )
	BW_CUSTOM_COPY_TO_PROJECT_OUTPUT( "${PROJECT_NAME}" "${_RESOURCES}" )
	ADD_DEFINITIONS( -DPROJECT_RESOURCE_FOLDER="plugins/${PROJECT_NAME}/" )
ENDMACRO()

# Copy any library resource files to a folder named after the library
MACRO( BW_COPY_LIBRARY_RESOURCES _RESOURCES )
	SOURCE_GROUP( "Resource Files" FILES ${_RESOURCES} )
	BW_GET_EXECUTABLE_DST_DIR( EXE_DIR )
    BW_GET_TEST_DST_DIR( TEST_DIR )
    FOREACH( resFile ${_RESOURCES} )
		GET_FILENAME_COMPONENT(_fileName ${resFile} NAME)
		ADD_CUSTOM_COMMAND(
			TARGET ${PROJECT_NAME}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
				"${resFile}"
				"${EXE_DIR}/plugins/${PROJECT_NAME}/${_fileName}"
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
				"${resFile}"
				"${TEST_DIR}/plugins/${PROJECT_NAME}/${_fileName}" )
	ENDFOREACH()
	ADD_DEFINITIONS( -DPROJECT_RESOURCE_FOLDER="plugins/${PROJECT_NAME}/" )
ENDMACRO()

MACRO( BW_CUSTOM_COPY_TO_PROJECT_OUTPUT _TARGET_DIR _RESOURCES )
    FOREACH( resFile ${_RESOURCES} )
		GET_FILENAME_COMPONENT(_fileName ${resFile} NAME)
		ADD_CUSTOM_COMMAND( OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${_TARGET_DIR}/${_fileName}"
			# Copy the file to our desired location
			COMMAND ${CMAKE_COMMAND} -E copy_if_different "${resFile}" $<TARGET_FILE_DIR:${PROJECT_NAME}>/${_TARGET_DIR}/${_fileName}
			COMMENT "Verifying ${resFile} in target directory: ${_TARGET_DIR} ..."
			MAIN_DEPENDENCY "${resFile}"
			VERBATIM
		)
    ENDFOREACH()
ENDMACRO()

# Get list of all dependency targets for all specified targets.
FUNCTION( BW_LIST_DEPENDENCY_TARGETS OUTPUT_LIST )
	FOREACH( SOURCE_TARGET ${ARGN} )
		GET_TARGET_PROPERTY( LIBS ${SOURCE_TARGET} LINK_LIBRARIES )
		FOREACH( LIB ${LIBS} )
			IF( TARGET ${LIB} )
				GET_TARGET_PROPERTY( LIB_TYPE ${LIB} TYPE )

				IF( "${LIB_TYPE}" STREQUAL "SHARED_LIBRARY" OR
					"${LIB_TYPE}" STREQUAL "MODULE_LIBRARY"
				)
					# add LIB itself
					LIST( APPEND TMP_LIST ${LIB} )

					# add LIB's dependencies
					BW_LIST_DEPENDENCY_TARGETS( RECURSIVE_LIBS ${LIB} )
					LIST( APPEND TMP_LIST ${RECURSIVE_LIBS} )
				ENDIF()
			ENDIF()
		ENDFOREACH()
	ENDFOREACH()

	IF( DEFINED TMP_LIST )
		LIST( REMOVE_DUPLICATES TMP_LIST )
	ENDIF()

	SET( ${OUTPUT_LIST} "${TMP_LIST}" PARENT_SCOPE )
ENDFUNCTION()

# Deploy runtime dependencies which are targets to the specified target's output.
# Synopsis: BW_DEPLOY_DEPENDENCY_TARGETS( SOURCE_TARGET [ DESTINATION_TARGET ] )
# SOURCE_TARGET - target to scan dependencies
# DESTINATION_TARGET - target to determine deployment directory; defaults to SOURCE_TARGET
FUNCTION( BW_DEPLOY_DEPENDENCY_TARGETS SOURCE_TARGET )
	IF( ARGN )
		LIST( GET ARGN 0 DESTINATION_TARGET )
	ELSE()
		SET( DESTINATION_TARGET "${SOURCE_TARGET}" )
	ENDIF()

	BW_LIST_DEPENDENCY_TARGETS( LIBS ${SOURCE_TARGET} )

	FOREACH( LIB ${LIBS} )
		BW_COPY_TARGET( "${DESTINATION_TARGET}" "${LIB}" )
	ENDFOREACH()
ENDFUNCTION()

function(red_auto_source_groups)
	set(_groups)

	foreach(_filename ${ARGN})
		get_filename_component(_dirname ${_filename} PATH)
		string(REPLACE "/" "_" _group_name "${_dirname}")
		set(_group_name "group_${_group_name}")

		if (${_group_name})
			list(APPEND ${_group_name} ${_filename})
		else()
			set(${_group_name} ${_filename})
			list(APPEND _groups ${_group_name})
		endif()
	endforeach()

	foreach (_group_name ${_groups})
		string(REGEX REPLACE "^group_" "" _group_label "${_group_name}")
		string(REPLACE "_" "\\" _group_label "${_group_label}")
		source_group("${_group_label}" FILES ${${_group_name}})
	endforeach()
endfunction()


# Get rid of it later (v_stepano : a_pirogov added this)

MACRO( WG_PROJECT_CATEGORY _PROJNAME _CATEGORY )
	SET_PROPERTY( TARGET ${_PROJNAME} PROPERTY FOLDER ${_CATEGORY} )
ENDMACRO( WG_PROJECT_CATEGORY )

# Enables of disables feature for different platforms/configurations
# WG_CONFIGURE_FEATURE(<target> <feature_name>
#                      [PLATFORMS platform1 [platform2 ...]
#                      [CONFIGURATIONS configuration1 [configuration2 ...]
#                      [DEVELOPMENT])
#
# TODO: Documentation
# EXAMPLE: WG_CONFIGURE_FEATURE( ${PROJECT_NAME} WG_PROFILER DEVELOPMENT )
# NOTE: Supplied CONFIGURATIONS can be overriden by <feature_name>_CONFIGURATIONS variable
# NOTE: DEVELOPMENT features will only exist in configurations listed in DEBUG_CONFIGURATIONS property
FUNCTION( WG_CONFIGURE_FEATURE _TARGET _FEATURE )
	SET( _FEATURE_VARIABLE_VALUE ${${_FEATURE}} )
	SET( _SUPPORTED_CONFIGURATIONS "" )
	SET( _IS_DEVELOPMENT_FEATURE FALSE )

	IF( _FEATURE_VARIABLE_VALUE )
		IF( _FEATURE_VARIABLE_VALUE STREQUAL FORCE )
			# As you wish. Not guaranteed to work in this particular config or on this platform
			TARGET_COMPILE_DEFINITIONS( ${_TARGET} PUBLIC HAVE_${_FEATURE}=1 )
			RETURN()
		ELSE()
			CMAKE_PARSE_ARGUMENTS( _ARG "DEVELOPMENT" "" "CONFIGURATIONS;PLATFORMS" ${ARGN})

			SET( _IS_DEVELOPMENT_FEATURE "${_ARG_DEVELOPMENT}" )

			IF( NOT _ARG_CONFIGURATIONS )
				# Supported in every configuration
				SET( _SUPPORTED_CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES} )
			ELSE()
				SET( _SUPPORTED_CONFIGURATIONS ${_ARG_CONFIGURATIONS} )
			ENDIF()

			# Overriding supported configurations
			IF( DEFINED ${_FEATURE}_CONFIGURATIONS )
				SET( _SUPPORTED_CONFIGURATIONS ${${_FEATURE}_CONFIGURATIONS} )
			ELSE()
				# Disabling development features in release configurations
				IF( _ARG_DEVELOPMENT AND _SUPPORTED_CONFIGURATIONS )
					GET_PROPERTY( _DEBUG_CONFIGURATIONS GLOBAL PROPERTY DEBUG_CONFIGURATIONS )
					IF( _DEBUG_CONFIGURATIONS )
						SET( _RELEASE_CONFIGURATIONS ${_SUPPORTED_CONFIGURATIONS} )
						LIST( REMOVE_ITEM _RELEASE_CONFIGURATIONS ${_DEBUG_CONFIGURATIONS} )
						IF( _RELEASE_CONFIGURATIONS )
							LIST( REMOVE_ITEM _SUPPORTED_CONFIGURATIONS ${_RELEASE_CONFIGURATIONS} )
						ENDIF()
					ENDIF()
				ENDIF()
			ENDIF()

			IF( _ARG_PLATFORMS )
				LIST( FIND _ARG_PLATFORMS ${WG_PLATFORM} _PLATFORM_INDEX )
				IF( _PLATFORM_INDEX EQUAL -1 )
					# Current platform isn't supported
					SET( _SUPPORTED_CONFIGURATIONS "" )
				ENDIF()
			ENDIF()
		ENDIF()
	ENDIF()

	IF( _SUPPORTED_CONFIGURATIONS )
		FOREACH( _CONFIGURATION_TYPE IN LISTS CMAKE_CONFIGURATION_TYPES )
			LIST( FIND _SUPPORTED_CONFIGURATIONS ${_CONFIGURATION_TYPE} _FOUND_INDEX )
			IF( ${_FOUND_INDEX} GREATER -1 )
				SET( _FOUND_INDEX 1 )
			ELSE()
				SET( _FOUND_INDEX 0 )
			ENDIF()
			TARGET_COMPILE_DEFINITIONS( ${_TARGET} PUBLIC
				$<$<CONFIG:${_CONFIGURATION_TYPE}>:HAVE_${_FEATURE}=${_FOUND_INDEX}> )
		ENDFOREACH()
	ELSE()
		# Completely disabled
		TARGET_COMPILE_DEFINITIONS( ${_TARGET} PUBLIC HAVE_${_FEATURE}=0 )
	ENDIF()
ENDFUNCTION()

# Copy output from one target to output dir of another target
# **Important Note**
# http://www.cmake.org/cmake/help/v2.8.8/cmake.html#command:add_custom_command
# "Do not list the output in more than one independent target that may build in
# parallel or the two instances of the rule may conflict
# (instead use add_custom_target to drive the command and make the other
# targets depend on that one)."
FUNCTION( WG_COPY_TARGET _TO_TARGET _FROM_TARGET )
	ADD_CUSTOM_COMMAND(
		TARGET ${_TO_TARGET}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE:${_FROM_TARGET}>
			$<TARGET_FILE_DIR:${_TO_TARGET}>/$<TARGET_FILE_NAME:${_FROM_TARGET}>
		COMMENT "Copying from ${_FROM_TARGET} to ${_TO_TARGET}'s output dir." )
ENDFUNCTION()


# Generate a plugin list file for parsing into a WGTF application.
# Will add dependencies on all the specified targets to the current target
# Will output a plugin list file into the specified location with the given name
FUNCTION( WGTF_GENERATE_PLUGIN_LIST )
	# Parse arguments
	set(options "")
    set(oneValueArgs DESTINATION TARGET PREFIX)
    set(multiValueArgs PLUGINS)
    cmake_parse_arguments(PLUGIN_LIST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

	if (NOT PLUGIN_LIST_DESTINATION)
		MESSAGE( FATAL_ERROR "No DESTINATION parameter defined for plugin list generation")
		return()
	else()
		
		# Generate the plugin list to disk
		file(WRITE ${PLUGIN_LIST_DESTINATION} 
			"# AUTOMATICALLY GENERATED FILE! Do not edit this file.\n"
			"# List generated from: " ${CMAKE_CURRENT_LIST_FILE} "\n")
		foreach(plugin ${PLUGIN_LIST_PLUGINS})
			file(APPEND ${PLUGIN_LIST_DESTINATION} 
				${PLUGIN_LIST_PREFIX} ${plugin} "\n")
		endforeach()
	endif()

	if(PLUGIN_LIST_TARGET)
		add_dependencies(${PLUGIN_LIST_TARGET} ${PLUGIN_LIST_PLUGINS})
	endif()

ENDFUNCTION()
