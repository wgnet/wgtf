CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_reflection )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

SET( INTERFACES_SRCS
	interfaces/i_base_property.hpp
	interfaces/i_class_definition.hpp
	interfaces/i_class_definition_details.hpp
	interfaces/i_class_definition_modifier.hpp
	interfaces/i_definition_helper.hpp
	interfaces/i_enum_generator.hpp
	interfaces/i_reflection_controller.hpp
	interfaces/i_reflection_property_setter.hpp
	interfaces/i_method.hpp
)
SOURCE_GROUP( "Interfaces" FILES ${INTERFACES_SRCS} )

SET( ALL_SRCS
	object_handle.cpp
	object_handle.hpp
	object_handle_storage.cpp
	object_handle_storage.hpp
	object_handle_storage_shared.hpp
	object_handle_variant_storage.cpp
	object_handle_variant_storage.hpp
	base_property.cpp
	base_property.hpp
	base_property_with_metadata.cpp
	base_property_with_metadata.hpp
	class_definition.cpp
	class_definition.hpp
	definition_manager.cpp
	definition_manager.hpp
	function_property.hpp
	generic/base_generic_object.cpp
	generic/base_generic_object.hpp
	generic/generic_definition.cpp
	generic/generic_definition.hpp
	generic/generic_definition_helper.cpp
	generic/generic_definition_helper.hpp
	generic/generic_object.cpp
	generic/generic_object.hpp
	generic/generic_property.cpp
	generic/generic_property.hpp
	i_definition_manager.hpp
	i_object_manager.hpp
	mutable_vector.hpp
	object_manager.cpp
	object_manager.hpp
	property_accessor.cpp
	property_accessor.hpp
	property_accessor_listener.hpp
	property_iterator.cpp
	property_iterator.hpp
	property_storage.cpp
	property_storage.hpp
	reflected_method.hpp
	reflected_method_parameters.cpp
	reflected_method_parameters.hpp
	reflected_object.hpp
	reflected_property.hpp
	reflected_types.cpp
	reflected_types.hpp
	reflection_macros.hpp
	ref_object_id.hpp
	ref_object_id.cpp
	type_class_definition.hpp
	metadata/meta_base.cpp
	metadata/meta_base.hpp
	metadata/meta_impl.cpp
	metadata/meta_impl.hpp
	metadata/meta_types.cpp
	metadata/meta_types.hpp
	metadata/meta_utilities.cpp
	metadata/meta_utilities.hpp
	types/reflected_collection.hpp
	utilities/reflection_function_utilities.hpp
	utilities/reflection_method_utilities.hpp
	utilities/reflection_utilities.hpp
	utilities/reflection_utilities.cpp
	utilities/definition_helpers.hpp
)
SOURCE_GROUP( "Source" FILES ${ALL_SRCS} )


BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
	${INTERFACES_SRCS}
)
BW_ADD_LIBRARY( core_reflection ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( core_reflection PRIVATE
	core_variant
	core_logging
	wg_types
)

BW_PROJECT_CATEGORY( core_reflection "WGT Libs/Reflection" )
