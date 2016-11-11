CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )
PROJECT( core_data_model )

SET( ALL_SRCS
	metadata/i_active_filters_model.mpp
	metadata/i_breadcrumbs_model.mpp
	asset_browser/metadata/i_asset_browser_model20.mpp
	asset_browser/metadata/i_asset_browser_event_model.mpp
	asset_browser/metadata/i_asset_browser_model.mpp
	asset_browser/metadata/i_asset_browser_view_model.mpp
	dialog/dialog_model.mpp
	abstract_item.hpp
	abstract_item_model.hpp
	collection_model.cpp
	collection_model.hpp
	collection_model_old.cpp
	collection_model_old.hpp
	filtered_list_model.cpp
	filtered_list_model.hpp
	filtered_tree_model.cpp
	filtered_tree_model.hpp
	generic_list.hpp
	variant_list.cpp
	variant_list.hpp
	generic_tree_model.cpp
	generic_tree_model.hpp
	generic_tree_item.cpp
	generic_tree_item.hpp
	i_active_filters_model.hpp
	i_breadcrumbs_model.hpp
	i_list_model.cpp
	i_list_model.hpp
	i_tree_model.cpp
	i_tree_model.hpp
	i_item.hpp
	i_item_role.cpp
	i_item_role.hpp
	i_selection_handler.hpp
	simple_active_filters_model.cpp
	simple_active_filters_model.hpp
	value_change_notifier.hpp
	selection_handler.cpp
	selection_handler.hpp
	common_data_roles.hpp
	reflection/class_definition_model.cpp
	reflection/class_definition_model.hpp
	reflection/class_definition_model_new.cpp
	reflection/class_definition_model_new.hpp
	reflection/reflected_enum_model.cpp
	reflection/reflected_enum_model.hpp
	reflection/reflected_enum_model_new.cpp
	reflection/reflected_enum_model_new.hpp
	reflection/reflected_group_item.cpp
	reflection/reflected_group_item.hpp
	reflection/reflected_group_item_new.cpp
	reflection/reflected_group_item_new.hpp
	reflection/reflected_item.cpp
	reflection/reflected_item.hpp
	reflection/reflected_object_item.cpp
	reflection/reflected_object_item.hpp
	reflection/reflected_object_item_new.cpp
	reflection/reflected_object_item_new.hpp
	reflection/reflected_property_item.cpp
	reflection/reflected_property_item.hpp
	reflection/reflected_property_item_new.cpp
	reflection/reflected_property_item_new.hpp
	reflection/reflected_tree_item_new.cpp
	reflection/reflected_tree_item_new.hpp
	reflection/reflected_tree_model.cpp
	reflection/reflected_tree_model.hpp
	reflection/reflected_tree_model_new.cpp
	reflection/reflected_tree_model_new.hpp
	reflection/reflected_list.cpp
	reflection/reflected_list.hpp
	reflection/reflected_property_model.hpp
	reflection/reflected_property_model.cpp
	reflection/default_property_model_extensions.cpp
	reflection/default_property_model_extensions.hpp
	reflection/property_model_extensions.hpp
	reflection/property_model_extensions.cpp
	reflection/reflected_collection_item.hpp
	reflection/reflected_collection_item.cpp
	reflection/child_creator.hpp
	reflection/child_creator.cpp
	reflection/reflected_collection.cpp
	reflection/reflected_collection.hpp
	reflection_proto/property_list_model.cpp
	reflection_proto/property_list_model.hpp
	reflection_proto/property_tree_model.cpp
	reflection_proto/property_tree_model.hpp
	reflection_proto/reflected_tree_model.cpp
	reflection_proto/reflected_tree_model.hpp
	reflection_proto/reflected_property_item.cpp
	reflection_proto/reflected_property_item.hpp
	asset_browser/asset_browser_breadcrumbs_model.cpp
	asset_browser/asset_browser_breadcrumbs_model.hpp
	asset_browser/asset_browser_event_model.cpp
	asset_browser/asset_browser_event_model.hpp
	asset_browser/asset_browser_view_model.cpp
	asset_browser/asset_browser_view_model.hpp
	asset_browser/asset_list_model.cpp
	asset_browser/asset_list_model.hpp
	asset_browser/base_asset_object_item.cpp
	asset_browser/base_asset_object_item.hpp
	asset_browser/asset_browser_model20.cpp
	asset_browser/asset_browser_model20.hpp
	asset_browser/file_system_asset_browser_model.cpp
	asset_browser/file_system_asset_browser_model.hpp
	asset_browser/file_system_asset_presentation_provider.cpp
	asset_browser/file_system_asset_presentation_provider.hpp
	asset_browser/folder_tree_model.cpp
	asset_browser/folder_tree_model.hpp
	asset_browser/i_asset_object_item.hpp
	asset_browser/i_asset_browser_event_model.hpp
	asset_browser/i_asset_browser_model.hpp
	asset_browser/i_asset_browser_view_model.hpp
	asset_browser/i_asset_presentation_provider.hpp
	dialog/dialog_model.hpp
	dialog/dialog_model.cpp
	filtering/i_item_filter.hpp
	filtering/string_filter.hpp
	filtering/string_filter.cpp
	filtering/tokenized_string_filter.hpp
	filtering/tokenized_string_filter.cpp
	file_system/file_system_model.cpp
	file_system/file_system_model.hpp
)

FILE( GLOB LIBRARY_RESOURCES asset_browser/resources/* )

WG_AUTO_SOURCE_GROUPS(${ALL_SRCS})

BW_BLOB_SOURCES( BLOB_SRCS 
	${ALL_SRCS}
	${LIBRARY_RESOURCES} 
)

BW_ADD_LIBRARY( core_data_model  ${BLOB_SRCS} )

BW_COPY_LIBRARY_RESOURCES( "${LIBRARY_RESOURCES}" )

BW_TARGET_LINK_LIBRARIES( core_data_model INTERFACE
	core_reflection
	core_reflection_utils
)

BW_PROJECT_CATEGORY( core_data_model "WGT Libs/Data Model" )

