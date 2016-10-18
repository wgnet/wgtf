CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_qt_common )

ADD_DEFINITIONS( -DNOMINMAX )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

find_package(QT5)

include_directories(.)

SET( ALL_SRCS
	models/data_change_notifier.cpp
	models/data_change_notifier.hpp
	models/qt_collection_model.cpp
	models/qt_collection_model.hpp
	models/qt_item_data.cpp
	models/qt_item_data.hpp
	models/qt_item_model.cpp
	models/qt_item_model.hpp
	models/qt_model_helpers.cpp
	models/qt_model_helpers.hpp
	models/qt_model_macros.hpp
	models/wg_tree_model.cpp
	models/wg_tree_model.hpp
	models/wg_tree_list_adapter.cpp
	models/wg_tree_list_adapter.hpp
	models/wg_list_model.cpp
	models/wg_list_model.hpp
	models/role_provider.cpp
	models/role_provider.hpp
	models/buttons_model.hpp
	models/buttons_model.cpp
	models/qt_abstract_item_model.hpp
	models/qt_abstract_item_model.cpp
	models/wg_column_layout_proxy.cpp
	models/wg_column_layout_proxy.hpp
	models/wg_merge_proxy.cpp
	models/wg_merge_proxy.hpp
	models/wg_sort_filter_proxy.cpp
	models/wg_sort_filter_proxy.hpp
	models/wg_transpose_proxy.cpp
	models/wg_transpose_proxy.hpp
	models/adapters/i_list_adapter.cpp
	models/adapters/i_list_adapter.hpp
	models/adapters/child_list_adapter.cpp
	models/adapters/child_list_adapter.hpp
	models/adapters/column_list_adapter.cpp
	models/adapters/column_list_adapter.hpp
	models/adapters/indexed_adapter.hpp
	models/adapters/sequence_list_adapter.cpp
	models/adapters/sequence_list_adapter.hpp
	models/extensions/i_model_extension.cpp
	models/extensions/i_model_extension.hpp
	models/extensions/column_extension.cpp
	models/extensions/column_extension.hpp
	models/extensions/component_extension.cpp
	models/extensions/component_extension.hpp
	models/extensions/image_extension.cpp
	models/extensions/image_extension.hpp
	models/extensions/list_extension.cpp
	models/extensions/list_extension.hpp
	models/extensions/spreadsheet_extension.cpp
	models/extensions/spreadsheet_extension.hpp
	models/extensions/table_extension.cpp
	models/extensions/table_extension.hpp
	models/extensions/tree_extension.cpp
	models/extensions/tree_extension.hpp
	models/extensions/deprecated/asset_item_extension.cpp
	models/extensions/deprecated/asset_item_extension.hpp
	models/extensions/deprecated/column_extension_old.cpp
	models/extensions/deprecated/column_extension_old.hpp
	models/extensions/deprecated/component_extension_old.cpp
	models/extensions/deprecated/component_extension_old.hpp
	models/extensions/deprecated/selection_extension.cpp
	models/extensions/deprecated/selection_extension.hpp
	models/extensions/deprecated/tree_extension_old.cpp
	models/extensions/deprecated/tree_extension_old.hpp
	models/extensions/deprecated/value_extension.cpp
	models/extensions/deprecated/value_extension.hpp
	models/extensions/deprecated/thumbnail_extension.cpp
	models/extensions/deprecated/thumbnail_extension.hpp
	models/extensions/deprecated/i_model_extension_old.hpp
	models/extensions/deprecated/buttons_model_extension.hpp
	models/extensions/deprecated/buttons_model_extension.cpp
	models/extensions/deprecated/buttons_definition_extension.hpp
	models/extensions/deprecated/buttons_definition_extension.cpp
	models/extensions/deprecated/header_footer_text_extension.cpp
	models/extensions/deprecated/header_footer_text_extension.hpp
	helpers/qt_helpers.hpp
	helpers/qt_helpers.cpp
	helpers/wg_filtered_list_model.hpp
	helpers/wg_filtered_list_model.cpp
	helpers/wg_filtered_tree_model.hpp
	helpers/wg_filtered_tree_model.cpp
	helpers/wg_filter.hpp
	helpers/wg_string_filter.hpp
	helpers/wg_string_filter.cpp
	helpers/wg_tokenized_string_filter.hpp
	helpers/wg_tokenized_string_filter.cpp
    helpers/selection_helper.cpp
    helpers/selection_helper.hpp
	helpers/qml_component_loader_helper.hpp
	helpers/qml_component_loader_helper.cpp
	controls/wg_copy_controller.hpp
	controls/wg_copy_controller.cpp
	controls/wg_filedialog_qi.hpp
	controls/wg_filedialog_qi.cpp
	controls/wg_action.hpp
	controls/wg_action.cpp
	controls/wg_context_menu.hpp
	controls/wg_context_menu.cpp
	controls/wg_item_view.hpp
	controls/wg_item_view.cpp
	interfaces/i_check_filter.hpp
	shared_controls.hpp
	shared_controls.cpp
	i_qt_framework.hpp
	i_qt_type_converter.hpp
	i_qt_view.hpp
	image_qt_type_converter.hpp
	image_qt_type_converter.cpp
	model_qt_type_converter.hpp
	model_qt_type_converter.cpp
	string_qt_type_converter.hpp
	vector_qt_type_converter.hpp
	qml_component.cpp
	qml_component.hpp
	qml_view.cpp
	qml_view.hpp
	qml_dialog.cpp
	qml_dialog.hpp
	qt_image_provider.cpp
	qt_image_provider.hpp
	qt_image_provider_old.cpp
	qt_image_provider_old.hpp
	qt_connection_holder.hpp
	qt_connection_holder.cpp
	qt_new_handler.hpp
	qt_new_handler.cpp
	qt_palette.hpp
	qt_palette.cpp
    qt_component_finder.hpp
    qt_component_finder.cpp
	qt_default_spacing.hpp
	qt_global_settings.hpp
	qt_view.cpp
	qt_view.hpp
	qt_window.cpp
	qt_window.hpp
	qml_window.cpp
	qml_window.hpp
	qt_dock_region.cpp
	qt_dock_region.hpp
	qt_status_bar.cpp
	qt_status_bar.hpp
	qt_tab_region.cpp
	qt_tab_region.hpp
	qt_menu.cpp
	qt_menu.hpp
	qt_menu_bar.cpp
	qt_menu_bar.hpp
	qt_tool_bar.cpp
	qt_tool_bar.hpp
	qt_context_menu.cpp
	qt_context_menu.hpp
	qt_qlist_memory_fix.hpp
	qt_progress_dialog.cpp
	qt_progress_dialog.hpp

	qt_private/qsortfilterproxymodel.cpp
	qt_private/qsortfilterproxymodel.h
)

WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_LIBRARY( core_qt_common ${BLOB_SRCS}
)

BW_TARGET_LINK_LIBRARIES( core_qt_common INTERFACE
	core_reflection
	core_data_model
	core_logging
	core_string_utils
	core_ui_framework
	core_qt_script
)
BW_TARGET_LINK_LIBRARIES( core_qt_common PRIVATE
	Qt5::Core
	Qt5::Gui
	Qt5::QuickWidgets
	Qt5::Quick
	Qt5::UiTools
	Qt5::Xml
)

INCLUDE_DIRECTORIES(${Qt5Core_PRIVATE_INCLUDE_DIRS})

BW_PROJECT_CATEGORY( core_qt_common "WGT Libs" )

