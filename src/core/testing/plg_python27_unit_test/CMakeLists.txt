CMAKE_MINIMUM_REQUIRED( VERSION 3.2.3 )
PROJECT( plg_python27_unit_test )

INCLUDE( WGPython27Project )
INCLUDE( WGTFMacros )
INCLUDE( WGMacros )
INCLUDE( WGPython27Common )

IF( NOT PYTHON_FOUND )
	RETURN()
ENDIF()

FIND_PACKAGE( QT5 ${QT_FIND_PACKAGE_VERSION} )

SET( PYTHON_SRCS
	resources/Scripts/python27_test/__init__.py
)
SOURCE_GROUP( "Python" FILES ${PYTHON_SRCS} )
BW_QT_ADD_QRC( Scripts resources/Scripts )
BW_GLOB_RESOURCE_FILES()

SET( ALL_SRCS
	pch.hpp
	pch.cpp
	plg_python27_unit_test.cpp
	python27_unit_test.cpp
	python27_unit_test.hpp
	reflection_test_module.cpp
	reflection_test_module.hpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

WG_BLOB_SOURCES( BLOB_SRCS ${ALL_SRCS} )
BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
	${BLOB_SRCS}
	${COMPILED_RESOURCES}
	${PYTHON_SRCS}
	${RESOURCE_FILES}
)
 
BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	core_generic_plugin
	core_reflection
	core_unit_test
	core_python27
	wg_pyscript
)

WGTF_TARGET_LINK_PYTHON( ${PROJECT_NAME} )

WG_PRECOMPILED_HEADER( ${PROJECT_NAME} pch.hpp )
BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )
