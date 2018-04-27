CMAKE_MINIMUM_REQUIRED( VERSION 3.2.3 )
PROJECT( wg_pyscript )

INCLUDE( WGPython27Project )
INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )
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

WG_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS} 
)
BW_ADD_LIBRARY( ${PROJECT_NAME} ${BLOB_SRCS} )

WGTF_TARGET_LINK_PYTHON( ${PROJECT_NAME} )

WG_PRECOMPILED_HEADER( ${PROJECT_NAME} pch.hpp )
BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Wargaming Libs" )

