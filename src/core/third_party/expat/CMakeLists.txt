CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
PROJECT( expat )

INCLUDE( WGToolsProject )
INCLUDE( WGToolsLibrary )

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} ) # for expat_config.h

BW_REMOVE_COMPILE_FLAGS( /WX )

INCLUDE( src/expat/ConfigureChecks.cmake )

FILE( GLOB EXPAT_SRCS
	src/expat/lib/*.c
	src/expat/lib/*.h
)

ADD_LIBRARY( expat
	${EXPAT_SRCS}
)

TARGET_COMPILE_DEFINITIONS( expat INTERFACE XML_STATIC )
TARGET_INCLUDE_DIRECTORIES( expat INTERFACE src/expat/lib )

BW_PROJECT_CATEGORY( expat "ThirdParty" )
