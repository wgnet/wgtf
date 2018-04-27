# Root level Wargaming CMake script. CMake should always be invoked at this
# level rather than on specific subdirectories. Specify what projects you
# want to generate using the BW_CMAKE_TARGET option, e.g.
#
# cmake -DBW_CMAKE_TARGET=client
#
CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )

# Ensure the BW_CMAKE_TARGET has been set
IF( NOT BW_CMAKE_TARGET )
	MESSAGE( FATAL_ERROR "BW_CMAKE_TARGET is not set, either configure CMake by running build/wg_cmake.bat or specify BW_CMAKE_TARGET when invoking CMake." )
ENDIF()

# Set the Wargaming source directory
SET( WG_TOOLS_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR} )
SET( WG_TOOLS_INTERFACE_DIR ${CMAKE_CURRENT_LIST_DIR}/core/interfaces )

# Still set BW_SOURCE_DIR here for just back compatibility, don't use BW_SOURCE_DIR for new code
SET( BW_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR} )

# Set serach location for CMake include files
SET( CMAKE_MODULE_PATH "${WG_TOOLS_SOURCE_DIR}/../build/cmake" )

IF ( NOT "${WG_HUB_DIR}" )
	SET( WG_HUB_DIR "${WG_TOOLS_SOURCE_DIR}/core/third_party/packages/")
ENDIF()

SET(WGTF_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}")
LIST( APPEND CMAKE_PREFIX_PATH "${WG_TOOLS_SOURCE_DIR}/../build/cmake/external_modules" )
LIST( APPEND CMAKE_PREFIX_PATH ${WG_HUB_DIR} )

SET(QT_FIND_PACKAGE_VERSION "${QT_VERSION}")
IF ("${QT_VERSION}" STREQUAL "5.6.1-1")
	SET(QT_FIND_PACKAGE_VERSION "5.6.1.1")
ENDIF()

# Configure wgBuild
FIND_PACKAGE( wgBuild )
WGBUILD_USE()

# Override the default CMake configs: Debug Release MinSizeRel RelWithDebInfo
# This must appear before ENABLE_LANGUAGE to override the defaults
SET( CMAKE_CONFIGURATION_TYPES Debug;Hybrid;Release
	CACHE STRING "Semicolon separated list of supported configuration types"
	FORCE )

# Include useful macros
INCLUDE( WGMacros )
INCLUDE( WGTFMacros )

IF (MSVC11)
    ADD_DEFINITIONS( -DDISABLE_CODE_FOR_2012 )
ENDIF()

# Detect clang-cl
IF( ${CMAKE_C_COMPILER_ID} MATCHES "Clang" )
	SET( CLANG_CL 1 )
ENDIF()

# Set compiler and linker options
INCLUDE( WGGlobalCompilerAndLinkerOptions )

# Ensure we have a functioning compiler. Work around CMake failing to detect
# that the compiler is unknown, do this by making sure there's some flags
IF( NOT CMAKE_CXX_FLAGS )
	MESSAGE( FATAL_ERROR "CMake failed to find a valid compiler!" )
ENDIF()

MESSAGE( STATUS "Compiler: ${BW_COMPILER_TOKEN}" )

PROJECT( ${BW_CMAKE_TARGET}_${BW_PLATFORM} )

# Binary output location
GET_FILENAME_COMPONENT( WG_TOOLS_BIN_DIR "${PROJECT_SOURCE_DIR}/../bin" REALPATH )

# Still set BW_GAME_DIR here for just back compatibility, don't use BW_GAME_DIR for new code
GET_FILENAME_COMPONENT( BW_GAME_DIR "${PROJECT_SOURCE_DIR}/../bin" REALPATH )

# Options
OPTION( WG_UNIT_TESTS_ENABLED "Enable unit tests" ON )

# Discover available Wargaming projects to generate. These are defined by
# WGConfiguration_<name>.cmake files in the cmake module path.
FILE( GLOB _target_cmake_files "${WG_TOOLS_SOURCE_DIR}/../build/cmake/WGConfiguration_*.cmake" )
FOREACH( _filename ${_target_cmake_files} )
	GET_FILENAME_COMPONENT( _target_name ${_filename} NAME_WE )
	STRING( REPLACE "WGConfiguration_" "" _target_name ${_target_name} )
	SET( BW_AVAILABLE_TARGETS ${BW_AVAILABLE_TARGETS} ${_target_name} )
ENDFOREACH()

# What are we generating?
LIST( FIND BW_AVAILABLE_TARGETS ${BW_CMAKE_TARGET} _exists )
IF( NOT ${_exists} EQUAL -1 )
	MESSAGE( STATUS "Generating Wargaming ${BW_CMAKE_TARGET}..." )
ELSE()
	MESSAGE( FATAL_ERROR "Unknown Wargaming CMake target '${BW_CMAKE_TARGET}'." )
ENDIF()

SET( BW_CMAKE_TARGET ${BW_CMAKE_TARGET}
	CACHE STRING "Type of Wargaming project to generate" )

# Set BW_IS_<name> variables
FOREACH( _targetname ${BW_AVAILABLE_TARGETS} )
	STRING( TOUPPER ${_targetname} _upper_targetname )
	SET( BW_IS_${_upper_targetname} OFF )
ENDFOREACH()

STRING( TOUPPER ${BW_CMAKE_TARGET} _upper_targetname )
# If this is a blob build, strip the suffix off the blob
STRING( REGEX REPLACE "_BLOB$" "" _upper_targetname "${_upper_targetname}" )
SET( BW_IS_${_upper_targetname} ON )

# Solution folders: http://athile.net/library/blog/?p=288
SET_PROPERTY( GLOBAL PROPERTY USE_FOLDERS ON )

# Default feature support, may be overridden by target configuration
SET( BW_PCH_SUPPORT ON )
SET( USE_MEMHOOK OFF )
SET( BW_FMOD_SUPPORT ON )
SET( BW_PYTHON_MODULE_SUPPORT OFF )
SET( BW_PYTHON_DLL_SUPPORT ON )
SET( BW_SCALEFORM_SUPPORT OFF )
SET( BW_SPEEDTREE_SUPPORT ON )
SET( BW_UMBRA_SUPPORT ON )
SET( NGT_SUPPORT OFF )
SET( CORE_PYTHON_SUPPORT OFF )
SET( Maya_SUPPORT OFF )
SET( Perforce_SUPPORT OFF )
SET( DEPLOY_QT_SUPPORT OFF )
SET( DEPLOY_PYTHON_SUPPORT OFF )
SET( ALL_THIRD_PARTY_REQUIRED OFF )
SET( QT_CUSTOM_DEPLOY_ARGS )
SET( FIND_CORE_PYTHON_PACKAGE ON )

# Include target specific configuration
INCLUDE( WGConfiguration_${BW_CMAKE_TARGET} )

# blob specific configuration
IF( BW_BLOB_CONFIG )
	ADD_DEFINITIONS( -DBW_BLOB_CONFIG=1 )
	IF( MSVC )
		IF( BW_PLATFORM STREQUAL "win64" )
			SET( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj" )
		ENDIF()
	ENDIF()
ENDIF()

# Add feature flags
IF( BW_PCH_SUPPORT )
	ADD_DEFINITIONS( -DPCH_SUPPORT=1 )
ENDIF()
IF( USE_MEMHOOK )
	ADD_DEFINITIONS( -DUSE_MEMHOOK=1 )
ENDIF()
IF( BW_FMOD_SUPPORT )
	ADD_DEFINITIONS( -DFMOD_SUPPORT=1 )
	SET( FMOD_INCLUDE_DIRS
		"${WG_TOOLS_SOURCE_DIR}/bw/third_party/fmod/api/inc"
		"${WG_TOOLS_SOURCE_DIR}/bw/third_party/fmod/fmoddesignerapi/api/inc" )
ENDIF()
IF( BW_PYTHON_MODULE_SUPPORT )
	# Build as *.pyd
	ADD_DEFINITIONS( -DBWCLIENT_AS_PYTHON_MODULE=1
		-DPYMODULE )
ENDIF()
IF( BW_PYTHON_DLL_SUPPORT )
	# Use existing python*.dll
	ADD_DEFINITIONS(
		-DPYTHON_IMPORT
	)
ENDIF()
IF( BW_PYTHON_DLL_SUPPORT AND BW_PYTHON_MODULE_SUPPORT )
	MESSAGE( FATAL_ERROR "Cannot have BW_PYTHON_DLL_SUPPORT and BW_PYTHON_MODULE_SUPPORT enabled at the same time." )
ENDIF()
IF( BW_SCALEFORM_SUPPORT )
	ADD_DEFINITIONS( -DSCALEFORM_SUPPORT=1 )
ENDIF()
IF( BW_SPEEDTREE_SUPPORT )
	ADD_DEFINITIONS( -DSPEEDTREE_SUPPORT=1 )
ENDIF()
IF( BW_UMBRA_SUPPORT )
	ADD_DEFINITIONS( -DUMBRA_ENABLE=1 )
ENDIF()
IF( NGT_SUPPORT )
	ADD_DEFINITIONS( -DENABLE_NGT_SUPPORT )
ENDIF()
IF( DEPLOY_PYTHON_SUPPORT )
	ADD_DEFINITIONS( -DENABLE_DEPLOY_PYTHON_SUPPORT )
ENDIF()
# Default exe with which Qt will be deployed
SET( DEPLOY_QT_TARGET_PROJECT generic_app )

# Setup CODE_INLINE for Hybrid and Consumer Release
SET_PROPERTY( DIRECTORY APPEND PROPERTY
	COMPILE_DEFINITIONS $<$<CONFIG:HYBRID>:CODE_INLINE> )
SET_PROPERTY( DIRECTORY APPEND PROPERTY
	COMPILE_DEFINITIONS $<$<CONFIG:CONSUMER_RELEASE>:CODE_INLINE> )

IF( CMAKE_BUILD_TYPE )
	MESSAGE( STATUS "Build configuration: ${CMAKE_BUILD_TYPE}" )
ELSE()
	MESSAGE( STATUS "Build configurations: ${CMAKE_CONFIGURATION_TYPES}" )
ENDIF()

BW_SET_DEBUG_POSTFIX( "_d" )
BW_SET_HYBRID_POSTFIX( "_h" )
BW_SET_RELEASE_POSTFIX( "" )
BW_SET_TEST_DST_DIR("${BW_GAME_DIR}/unit_tests/${BW_PLATFORM}")

IF( NGT_OUTPUT_DIR )
	MESSAGE( STATUS "Executable output dir set to: ${NGT_OUTPUT_DIR}" )
	BW_SET_EXECUTABLE_DST_DIR(${NGT_OUTPUT_DIR})
ELSE()
	BW_SET_EXECUTABLE_DST_DIR("${BW_GAME_DIR}/${TARGET_PLUGIN_APP_FOLDER_NAME}/${BW_PLATFORM}")
ENDIF()

# Add libraries
ARRAY2D_BEGIN_LOOP( _islooping "${BW_LIBRARY_PROJECTS}" 2 "libname;libpath" )
	WHILE( _islooping )
		MESSAGE( STATUS "Adding library: ${libname} from ${libpath}" )

		ADD_SUBDIRECTORY( ${libpath} )
		ARRAY2D_ADVANCE()
	ENDWHILE()
ARRAY2D_END_LOOP()

# Add executables
INCLUDE( WGProjectUserFile )
ARRAY2D_BEGIN_LOOP( _islooping "${BW_BINARY_PROJECTS}" 2 "projname;projpath" )
	WHILE( _islooping )
		MESSAGE( STATUS "Adding executable: ${projname} from ${projpath}" )
		ADD_SUBDIRECTORY( ${projpath} )

		# Always set the working directory to bin for every project
		BW_SET_WORKING_DIRECTORY( ${projname} ${projpath} "$(TargetDir)" )

		ARRAY2D_ADVANCE()
	ENDWHILE()
ARRAY2D_END_LOOP()

# Add plugins
ARRAY2D_BEGIN_LOOP( _islooping "${BW_PLUGIN_PROJECTS}" 2 "plugname;plugpath" )
	WHILE( _islooping )
		MESSAGE( STATUS "Adding plugin: ${plugname} from ${plugpath}" )
		ADD_SUBDIRECTORY( ${plugpath} )

		# Always generate debug symbols, even in Release builds
		IF( MSVC )
			BW_APPEND_TARGET_PROPERTIES( ${plugname} LINK_FLAGS_HYBRID "/DEBUG" )
			BW_APPEND_TARGET_PROPERTIES( ${plugname} LINK_FLAGS_RELEASE "/DEBUG" )
			BW_APPEND_TARGET_PROPERTIES( ${plugname} LINK_FLAGS_CONSUMER_RELEASE "/DEBUG" )
		ENDIF( MSVC )

		# Always set the working directory to bin for every project
		BW_SET_WORKING_DIRECTORY( ${plugname} ${plugpath} "$(TargetDir)" )

		ARRAY2D_ADVANCE()
	ENDWHILE()
ARRAY2D_END_LOOP()

# Add external libraries
ARRAY2D_BEGIN_LOOP( _islooping "${BW_EXTERN_LIBRARY_PROJECTS}" 2 "libname;libpath" )
	WHILE( _islooping )
		MESSAGE( STATUS "Adding library: ${libname} from ${libpath}" )

		ADD_SUBDIRECTORY( ${libpath} ./core/extern_libs/${libname})
		ARRAY2D_ADVANCE()
	ENDWHILE()
ARRAY2D_END_LOOP()

# Add external executables
INCLUDE( WGProjectUserFile )
ARRAY2D_BEGIN_LOOP( _islooping "${BW_EXTERN_BINARY_PROJECTS}" 2 "projname;projpath" )
	WHILE( _islooping )
		MESSAGE( STATUS "Adding executable: ${projname} from ${projpath}" )
		ADD_SUBDIRECTORY( ${projpath} ./core/extern_bin/${projname})

		# Always set the working directory to bin for every project
		BW_SET_WORKING_DIRECTORY( ${projname} ${projpath} "$(TargetDir)" )

		ARRAY2D_ADVANCE()
	ENDWHILE()
ARRAY2D_END_LOOP()

# Add external plugins
ARRAY2D_BEGIN_LOOP( _islooping "${BW_EXTERN_PLUGIN_PROJECTS}" 2 "plugname;plugpath" )
	WHILE( _islooping )
		MESSAGE( STATUS "Adding plugin: ${plugname} from ${plugpath}" )
		ADD_SUBDIRECTORY( ${plugpath} ./core/custom_plugins/${plugname} )

		# Always generate debug symbols, even in Release builds
		IF( MSVC )
			BW_APPEND_TARGET_PROPERTIES( ${plugname} LINK_FLAGS_HYBRID "/DEBUG" )
			BW_APPEND_TARGET_PROPERTIES( ${plugname} LINK_FLAGS_RELEASE "/DEBUG" )
			BW_APPEND_TARGET_PROPERTIES( ${plugname} LINK_FLAGS_CONSUMER_RELEASE "/DEBUG" )
		ENDIF( MSVC )

		# Always set the working directory to bin for every project
		BW_SET_WORKING_DIRECTORY( ${plugname} ${plugpath} "$(TargetDir)" )

		ARRAY2D_ADVANCE()
	ENDWHILE()
ARRAY2D_END_LOOP()
