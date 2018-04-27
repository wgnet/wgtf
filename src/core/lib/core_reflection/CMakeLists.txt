CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_reflection )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )
INCLUDE( ExtractExports )

ADD_DEFINITIONS( -DREFLECTION_EXPORT )
ADD_DEFINITIONS( -DCORE_REFLECTION )

SET( ALL_SRCS
	interfaces/i_base_property.hpp
	interfaces/i_class_definition.hpp
	interfaces/i_class_definition_details.hpp
	interfaces/i_class_definition_modifier.hpp
	interfaces/i_definition_helper.hpp
	interfaces/i_enum_generator.hpp
	interfaces/i_signal_provider.hpp
	interfaces/i_reflection_controller.hpp
	interfaces/i_reflection_property_setter.hpp
	interfaces/i_method.hpp
	interfaces/i_direct_base_helper.hpp
	interfaces/i_property_path.hpp
	object_handle.hpp
	base_property.cpp
	base_property.hpp
	base_property_with_metadata.hpp
	base_property_with_metadata.cpp
	class_definition.cpp
	class_definition.hpp
	definition_manager.cpp
	direct_base_helper.hpp
	definition_manager.hpp
	function_property.hpp
	generic/generic_object.hpp
	generic/base_generic_object.cpp
	generic/base_generic_object.hpp
	generic/generic_definition.cpp
	generic/generic_definition.hpp
	generic/generic_definition_helper.cpp
	generic/generic_definition_helper.hpp
	generic/generic_property.cpp
	generic/generic_property.hpp
	i_definition_manager.hpp
	i_object_manager.hpp
	lambda_property.cpp
	lambda_property.hpp
	memory_overrides.cpp
	property_accessor.cpp
	property_accessor.hpp
	property_accessor_listener.hpp
	property_iterator.cpp
	property_iterator.hpp
	property_storage.cpp
	property_storage.hpp
	reflected_method.hpp
	reflection_batch_query.hpp
	reflection_batch_query.cpp
	reflected_method_parameters.cpp
	reflected_method_parameters.hpp
	reflected_object.hpp
	reflected_property.hpp
	reflection_dll.hpp
	reflection_dll.cpp
	reflection_macros.hpp
	ref_object_id.hpp
	ref_object_id.cpp
	ref_object_ids.hpp
	ref_object_ids.cpp
	type_class_definition.hpp
	managed_object_base.hpp
	managed_object_base.cpp
	metadata/i_meta_type_creator.hpp
	metadata/meta_base.hpp
	metadata/meta_base.cpp
	metadata/meta_impl.cpp
	metadata/meta_impl.hpp
	metadata/meta_types.hpp
	metadata/meta_type_creator.hpp
	metadata/meta_utilities.hpp
	metadata/meta_utilities_impl.hpp
	metadata/meta_command_executable.hpp
	metadata/meta_expose.mpp
	metadata/meta_type_creator.hpp
	metadata/i_meta_type_creator.hpp
	meta_callback_property_accessor_listener.cpp
	meta_callback_property_accessor_listener.hpp
	object/object_handle.hpp
	object/object_handle.cpp
	object/object_reference.cpp
	object/object_reference.hpp
	types/signal_provider_impl.hpp
	types/signal_provider_impl.cpp 
	utilities/counter_helper.hpp
	utilities/reflection_auto_register.hpp
	utilities/reflection_function_utilities.hpp
	utilities/reflection_method_utilities.hpp
	utilities/reflection_utilities.hpp
	utilities/reflection_utilities.cpp
	utilities/definition_helpers.hpp
	utilities/object_handle_reflection_utils.hpp
	utilities/object_handle_reflection_utils.cpp
	private/internal_context_query.hpp
	private/reflection_cache.hpp
	private/reflection_cache.cpp
	private/property_accessor_data.hpp
	private/property_accessor_data.cpp
	private/property_path.hpp
	private/property_path.cpp
)

WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

IF( "${CMAKE_SYSTEM_NAME}" STREQUAL "Windows" )
	SET( EXPORTS_SRC ${CMAKE_CURRENT_BINARY_DIR}/reflection_exports.cpp )
	SOURCE_GROUP( "Generated" FILES ${EXPORTS_SRC} )

	EXTRACT_EXPORTS( ${EXPORTS_SRC}
		core_variant
		core_serialization
	)
ELSE()
	UNSET( EXPORTS_SRC )
ENDIF()

SET( EXPORT_DEFINES REFLECTION_EXPORT VARIANT_EXPORT SERIALIZATION_EXPORT )

WG_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
	${EXPORTS_SRC}
)
BW_ADD_LIBRARY( core_reflection SHARED ${BLOB_SRCS} )

IF( "${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin" )
	BW_TARGET_LINK_LIBRARIES( core_reflection PRIVATE
		-Wl,-force_load core_variant
		-Wl,-force_load core_serialization
	)
ENDIF()

BW_TARGET_LINK_LIBRARIES( core_reflection PRIVATE
	core_variant
	core_serialization
	core_dependency_system
	core_logging
	wgtf_memory
	wgtf_types
	core_object
	ole32.lib
)

FOREACH( lib core_variant core_serialization core_reflection )
	BW_APPEND_TARGET_LIST_PROPERTIES( ${lib} COMPILE_DEFINITIONS
		${EXPORT_DEFINES}
	)
ENDFOREACH()

BW_PROJECT_CATEGORY( core_reflection "WGT Libs/Reflection" )
