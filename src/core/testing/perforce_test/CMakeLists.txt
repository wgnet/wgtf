CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( perforce_test )

INCLUDE( WGToolsProject )

WG_FIND_PACKAGE( Perforce "15.2" )

IF( NOT PERFORCE_FOUND )
	# Use ADD_CUSTOM_TARGET because this target has no cpp files
	ADD_CUSTOM_TARGET( ${PROJECT_NAME} COMMENT "${PROJECT_NAME} disabled." )
	BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Disabled" )
	RETURN()
ENDIF()

# Setup Visual Studio Unit Test Include Paths
IF( ${CMAKE_GENERATOR} MATCHES "Visual Studio 11( 2012)?" )
	SET(VCINSTALLDIR $ENV{VS110COMNTOOLS}/../../VC/)
ELSEIF( ${CMAKE_GENERATOR} MATCHES "Visual Studio 12( 2013)?" )
	SET(VCINSTALLDIR $ENV{VS120COMNTOOLS}/../../VC/)
ELSEIF( ${CMAKE_GENERATOR} MATCHES "Visual Studio 14( 2015)?" )
    SET(VCINSTALLDIR $ENV{VS140COMNTOOLS}/../../VC/)
ENDIF()
INCLUDE_DIRECTORIES( ${VCINSTALLDIR}UnitTest/include )
LINK_DIRECTORIES( "${VCINSTALLDIR}UnitTest/lib/" )

# Find includes in corresponding build directories
SET( CMAKE_INCLUDE_CURRENT_DIR ON )

INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/core/interfaces/ )

SET( ALL_SRCS
	stdafx.cpp
	stdafx.h
	perforce_test.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_LIBRARY( perforce_test SHARED ${BLOB_SRCS} )
BW_PRECOMPILED_HEADER( perforce_test stdafx.h )

BW_TARGET_LINK_LIBRARIES( perforce_test PRIVATE
	core_generic_plugin_manager
	Microsoft.VisualStudio.TestTools.CppUnitTestFramework
)

BW_GET_EXECUTABLE_DST_DIR(EXE_DIR)
BW_SET_BINARY_DIR( perforce_test "${EXE_DIR}" )
BW_PROJECT_CATEGORY( perforce_test "Unit Tests" )

