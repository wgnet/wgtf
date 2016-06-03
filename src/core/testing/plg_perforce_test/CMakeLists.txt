CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( test_plg_perforce )

IF(TARGET plg_wgs_core)
    ADD_DEPENDENCIES( test_plg_perforce plg_wgs_core )
ELSE()
    RETURN()
ENDIF()

INCLUDE( BWStandardProject )

# Setup Visual Studio Unit Test Include Paths
IF( ${CMAKE_GENERATOR} MATCHES "Visual Studio 11( 2012)?" )
	SET(VCINSTALLDIR $ENV{VS110COMNTOOLS}/../../VC/)
ELSEIF( ${CMAKE_GENERATOR} MATCHES "Visual Studio 12( 2013)?" )
	SET(VCINSTALLDIR $ENV{VS120COMNTOOLS}/../../VC/)
ENDIF()
INCLUDE_DIRECTORIES( ${VCINSTALLDIR}UnitTest/include )
LINK_DIRECTORIES( "${VCINSTALLDIR}UnitTest/lib/" )

REMOVE_DEFINITIONS( /WX )
ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS )

ADD_DEFINITIONS( 
	-DCSTDMF_IMPORT
)

# Find includes in corresponding build directories
SET( CMAKE_INCLUDE_CURRENT_DIR ON )

INCLUDE_DIRECTORIES( ${BW_SOURCE_DIR}/core/interfaces/ )

SET( ALL_SRCS
	stdafx.cpp
	stdafx.h
	test_plg_perforce.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_LIBRARY( test_plg_perforce SHARED ${BLOB_SRCS} )
BW_PRECOMPILED_HEADER( test_plg_perforce stdafx.h )

BW_TARGET_LINK_LIBRARIES( test_plg_perforce PRIVATE
	generic_plugin_manager
	Microsoft.VisualStudio.TestTools.CppUnitTestFramework
)

BW_GET_EXECUTABLE_DST_DIR(EXE_DIR)
BW_SET_BINARY_DIR( test_plg_perforce "${EXE_DIR}" )
BW_PROJECT_CATEGORY( test_plg_perforce "Unit Tests" )

