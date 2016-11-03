CMAKE_MINIMUM_REQUIRED( VERSION 3.2.3 )
PROJECT( wg_pyscript )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )
INCLUDE( WGPython27Project )
IF( NOT PYTHON_FOUND )
	RETURN()
ENDIF()

SET( ALL_SRCS
	pch.cpp
	pch.hpp
	py_script_args.ipp
	py_script_dict.ipp
	py_script_list.ipp
	py_script_module.ipp
	py_script_object.cpp
	py_script_object.hpp
	py_script_output_writer.cpp
	py_script_output_writer.hpp
	py_script_sequence.ipp
	py_script_tuple.ipp
	py_script_type.ipp
	pyobject_pointer.hpp
	type_converter.cpp
	type_converter.hpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS} 
)
BW_ADD_LIBRARY( ${PROJECT_NAME} ${BLOB_SRCS} )

WG_TARGET_LINK_PYTHON()

BW_PRECOMPILED_HEADER( ${PROJECT_NAME} pch.hpp )
BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Wargaming Libs" )

