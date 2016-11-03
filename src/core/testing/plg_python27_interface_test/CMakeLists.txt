CMAKE_MINIMUM_REQUIRED( VERSION 3.2.3 )
PROJECT( plg_python27_interface_test )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGPython27Common )

FIND_PACKAGE( QT5 )

SET( PYTHON_SRCS
	resources/Scripts/python27_test/__init__.py
)
SOURCE_GROUP( "Python" FILES ${PYTHON_SRCS} )
BW_QT_ADD_QRC( Scripts resources/Scripts )
BW_GLOB_RESOURCE_FILES()

SET( ALL_SRCS
	main_application.cpp
	main_application.hpp
	pch.cpp
	pch.hpp
	plg_python27_interface_test.cpp
	python27_interface_test.cpp
	python27_interface_test.hpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
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
	Qt5::Core
)

BW_PRECOMPILED_HEADER( ${PROJECT_NAME} pch.hpp )
BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )

