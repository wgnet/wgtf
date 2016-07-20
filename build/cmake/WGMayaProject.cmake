INCLUDE( WGStandardProject )

IF (NOT EXISTS("${MAYA_THIRD_PARTY_DIR}"))

	IF( NOT MAYA_VERSION )
	    SET( MAYA_VERSION "2014" )
	ENDIF()

	SET( MAYA_THIRD_PARTY_DIR ${WG_TOOLS_THIRD_PARTY_DIR}/maya/${MAYA_VERSION} )
ENDIF()

IF ( NOT EXISTS "${MAYA_THIRD_PARTY_DIR}" )
    SET( MAYA_THIRD_PARTY_NOT_FOUND ON )
    RETURN()
ELSE()
    SET( MAYA_THIRD_PARTY_FOUND ON )
ENDIF()

# Find Maya
SET( MAYA_DEVKIT_DIR ${MAYA_THIRD_PARTY_DIR} )

SET( MAYA_QT_DIR ${MAYA_DEVKIT_DIR}/qt )
SET( MAYA_QT_HEADER_DIR ${MAYA_QT_DIR}/include )
SET( MAYA_INCLUDE_DIRS ${MAYA_DEVKIT_DIR}/include)

SET( ENV{MAYA_LOCATION} ${MAYA_DEVKIT_DIR} )
SET( MAYA_EXECUTABLE ${MAYA_DEVKIT_DIR}/bin/maya.exe )

FIND_PACKAGE( Maya ${MAYA_VERSION} REQUIRED )

# Help FindQt4.cmake find the Maya Qt
SET(QT_HEADERS_DIR ${MAYA_QT_HEADER_DIR})

FIND_PATH(QT_QTCORE_INCLUDE_DIR QtCore
        HINTS ${QT_HEADERS_DIR}
        PATH_SUFFIXES QtCore
        NO_DEFAULT_PATH
)

SET(QT_QMAKE_EXECUTABLE ${MAYA_QT_DIR}/bin/qmake.exe)

FIND_PACKAGE( Qt4 ${MAYA_QT_VERSION_SHORT} REQUIRED QtCore QtGui )

# load the Qt tools
INCLUDE( ${QT_USE_FILE} )

INCLUDE_DIRECTORIES( ${MAYA_DEVKIT_DIR}/include )
INCLUDE_DIRECTORIES( ${MAYA_QT_HEADER_DIR} )
