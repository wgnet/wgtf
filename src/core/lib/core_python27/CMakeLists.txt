CMAKE_MINIMUM_REQUIRED( VERSION 3.2.3 )
PROJECT( core_python27 )

INCLUDE( WGPython27Project )
INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )
INCLUDE( WGTFMacros )
INCLUDE( WGMacros )

IF( NOT PYTHON_FOUND )
	RETURN()
ENDIF()

SET( ALL_SRCS
	type_converters/converter_queue.cpp
	type_converters/converter_queue.hpp
	type_converters/converters.cpp
	type_converters/converters.hpp
	type_converters/default_converter.cpp
	type_converters/default_converter.hpp
	type_converters/dict_converter.cpp
	type_converters/dict_converter.hpp
	type_converters/i_type_converter.hpp
	type_converters/i_parent_type_converter.hpp
	type_converters/list_converter.cpp
	type_converters/list_converter.hpp
	type_converters/mapping_collection.cpp
	type_converters/mapping_collection.hpp
	type_converters/mapping_iterator.cpp
	type_converters/mapping_iterator.hpp
	type_converters/none_converter.cpp
	type_converters/none_converter.hpp
	type_converters/primitive_converter.cpp
	type_converters/primitive_converter.hpp
	type_converters/sequence_collection.cpp
	type_converters/sequence_collection.hpp
	type_converters/sequence_iterator.cpp
	type_converters/sequence_iterator.hpp
	type_converters/tuple_converter.cpp
	type_converters/tuple_converter.hpp
	defined_instance.cpp
	defined_instance.hpp
	definition_details.cpp
	definition_details.hpp
	definition_helper.cpp
	definition_helper.hpp
	listener_hooks.cpp
	listener_hooks.hpp
	pch.cpp
	pch.hpp
	property.cpp
	property.hpp
	scripting_engine.cpp
	scripting_engine.hpp
    private/python_importer.cpp
    private/python_importer.hpp
	i_script_object_definition_registry.hpp
	script_object_definition_registry.cpp
	script_object_definition_registry.hpp
	python_obj_manager.hpp
	python_obj_manager.cpp
	interfaces/i_python_obj_manager.hpp
)

WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

WG_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_LIBRARY( ${PROJECT_NAME} ${BLOB_SRCS} )
 
BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} INTERFACE
	core_dependency_system
	core_reflection
	wgtf_types
	wg_pyscript
)

WGTF_TARGET_LINK_PYTHON( ${PROJECT_NAME} )

WG_PRECOMPILED_HEADER( ${PROJECT_NAME} pch.hpp )
BW_PROJECT_CATEGORY( ${PROJECT_NAME} "WGT Libs" )
