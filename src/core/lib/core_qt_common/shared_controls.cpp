#include "shared_controls.hpp"

#include "controls/wg_filedialog_qi.hpp"
#include "controls/wg_copy_controller.hpp"
#include "controls/wg_context_menu.hpp"
#include "controls/wg_action.hpp"
#include "controls/wg_item_view.hpp"
#include "helpers/wg_filtered_list_model.hpp"
#include "helpers/wg_filtered_tree_model.hpp"
#include "helpers/wg_filter.hpp"
#include "helpers/wg_string_filter.hpp"
#include "helpers/wg_tokenized_string_filter.hpp"
#include "models/adapters/sequence_list_adapter.hpp"
#include "models/data_change_notifier.hpp"
#include "models/extensions/column_extension.hpp"
#include "models/extensions/component_extension.hpp"
#include "models/extensions/list_extension.hpp"
#include "models/extensions/table_extension.hpp"
#include "models/extensions/tree_extension.hpp"
#include "models/extensions/image_extension.hpp"
#include "models/extensions/spreadsheet_extension.hpp"
#include "models/extensions/deprecated/asset_item_extension.hpp"
#include "models/extensions/deprecated/column_extension_old.hpp"
#include "models/extensions/deprecated/component_extension_old.hpp"
#include "models/extensions/deprecated/selection_extension.hpp"
#include "models/extensions/deprecated/thumbnail_extension.hpp"
#include "models/extensions/deprecated/tree_extension_old.hpp"
#include "models/extensions/deprecated/value_extension.hpp"
#include "models/extensions/deprecated/buttons_definition_extension.hpp"
#include "models/extensions/deprecated/buttons_model_extension.hpp"
#include "models/extensions/deprecated/header_footer_text_extension.hpp"
#include "models/wg_list_model.hpp"
#include "models/wg_tree_model.hpp"
#include "models/wg_tree_list_adapter.hpp"
#include "models/wg_column_layout_proxy.hpp"
#include "models/wg_sort_filter_proxy.hpp"
#include "models/wg_transpose_proxy.hpp"
#include "models/wg_merge_proxy.hpp"
#include "models/wg_sub_proxy.hpp"
#include "helpers/selection_helper.hpp"
#include "core_data_model/asset_browser/i_asset_browser_model.hpp"
#include "core_data_model/asset_browser/i_asset_browser_view_model.hpp"
#include "core_data_model/asset_browser/i_asset_browser_event_model.hpp"
#include "core_data_model/asset_browser/metadata/i_asset_browser_model20.mpp"
#include "core_data_model/asset_browser/metadata/i_asset_browser_model.mpp"
#include "core_data_model/asset_browser/metadata/i_asset_browser_event_model.mpp"
#include "core_data_model/asset_browser/metadata/i_asset_browser_view_model.mpp"
#include "core_data_model/i_active_filters_model.hpp"
#include "core_data_model/i_breadcrumbs_model.hpp"
#include "core_data_model/metadata/i_active_filters_model.mpp"
#include "core_data_model/metadata/i_breadcrumbs_model.mpp"
#include "core_data_model/dialog/dialog_model.hpp"
#include "core_data_model/dialog/dialog_model.mpp"

#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/reflection_macros.hpp"

#include "register_type.hpp"
#include "core_ui_framework/i_ui_framework.hpp"
#include "core_dependency_system/i_interface.hpp"

#include <QtQuick>

namespace wgt
{
//==============================================================================
/*static */ void SharedControls::init()
{
	auto uiFramework = Context::queryInterface<IUIFramework>();
	// DEPRECATED BW Controls
	registerType<DataChangeNotifier>(uiFramework, "BWControls", 1, 0, "BWDataChangeNotifier");
	registerType<SequenceListAdapter>(uiFramework, "BWControls", 1, 0, "SequenceList");
	registerType<WGCopyController>(uiFramework, "BWControls", 1, 0, "BWCopyable");
	registerType<WGFileDialogQI>(uiFramework, "BWControls", 1, 0, "BWFileDialogQI");

	// WG Controls 1.x
	registerType<AssetItemExtension>(uiFramework, "WGControls", 1, 0, "AssetItemExtension");
	registerType<ColumnExtensionOld>(uiFramework, "WGControls", 1, 0, "ColumnExtension");
	registerType<ComponentExtensionOld>(uiFramework, "WGControls", 1, 0, "ComponentExtension");
	registerType<DataChangeNotifier>(uiFramework, "WGControls", 1, 0, "WGDataChangeNotifier");
	registerType<HeaderFooterTextExtension>(uiFramework, "WGControls", 1, 0, "HeaderFooterTextExtension");
	registerType<SelectionExtension>(uiFramework, "WGControls", 1, 0, "SelectionExtension");
	registerType<SelectionHelper>(uiFramework, "WGControls", 1, 0, "WGSelectionHelper");
	registerType<SequenceListAdapter>(uiFramework, "WGControls", 1, 0, "WGSequenceList");
	registerType<ThumbnailExtension>(uiFramework, "WGControls", 1, 0, "ThumbnailExtension");
	registerType<TreeExtensionOld>(uiFramework, "WGControls", 1, 0, "TreeExtension");
	registerType<WGAction>(uiFramework, "WGControls", 1, 0, "WGAction");
	registerType<WGCopyController>(uiFramework, "WGControls", 1, 0, "WGCopyController");
	registerType<WGContextMenu>(uiFramework, "WGControls", 1, 0, "WGContextMenu");
	registerType<WGFileDialogQI>(uiFramework, "WGControls", 1, 0, "WGFileDialogQI");
	registerType<WGFilter>(uiFramework, "WGControls", 1, 0, "WGFilter");
	registerType<WGFilteredListModel>(uiFramework, "WGControls", 1, 0, "WGFilteredListModel");
	registerType<WGFilteredTreeModel>(uiFramework, "WGControls", 1, 0, "WGFilteredTreeModel");
	registerType<WGListModel>(uiFramework, "WGControls", 1, 0, "WGListModel");
	registerType<WGStringFilter>(uiFramework, "WGControls", 1, 0, "WGStringFilter");
	registerType<WGTokenizedStringFilter>(uiFramework, "WGControls", 1, 0, "WGTokenizedStringFilter");
	registerType<WGTreeModel>(uiFramework, "WGControls", 1, 0, "WGTreeModel");
	registerType<WGTreeListAdapter>(uiFramework, "WGControls", 1, 0, "WGTreeListAdapter");
	registerType<ValueExtension>(uiFramework, "WGControls", 1, 0, "ValueExtension");

	registerType<WGCopyController>(uiFramework, "WGCopyController", 1, 1, "WGCopyController", 1);
	// WG Controls 2.x
	// Exist from 1.0
	registerType<AssetItemExtension>(uiFramework, "WGControls", 2, 0, "AssetItemExtension");
	registerType<ColumnExtensionOld>(uiFramework, "WGControls", 2, 0, "ColumnExtension");
	registerType<ComponentExtensionOld>(uiFramework, "WGControls", 2, 0, "ComponentExtension");
	registerType<DataChangeNotifier>(uiFramework, "WGControls", 2, 0, "WGDataChangeNotifier");
	registerType<HeaderFooterTextExtension>(uiFramework, "WGControls", 2, 0, "HeaderFooterTextExtension");
	registerType<SelectionExtension>(uiFramework, "WGControls", 2, 0, "SelectionExtension");
	registerType<SelectionHelper>(uiFramework, "WGControls", 2, 0, "WGSelectionHelper");
	registerType<SequenceListAdapter>(uiFramework, "WGControls", 2, 0, "WGSequenceList");
	registerType<ThumbnailExtension>(uiFramework, "WGControls", 2, 0, "ThumbnailExtension");
	registerType<TreeExtensionOld>(uiFramework, "WGControls", 2, 0, "TreeExtension");
	registerType<WGAction>(uiFramework, "WGControls", 2, 0, "WGAction");
	registerType<WGCopyController>(uiFramework, "WGControls", 2, 0, "WGCopyController");
	registerType<WGContextMenu>(uiFramework, "WGControls", 2, 0, "WGContextMenu");
	registerType<WGFileDialogQI>(uiFramework, "WGControls", 2, 0, "WGFileDialogQI");
	registerType<WGFilter>(uiFramework, "WGControls", 2, 0, "WGFilter");
	registerType<WGFilteredListModel>(uiFramework, "WGControls", 2, 0, "WGFilteredListModel");
	registerType<WGFilteredTreeModel>(uiFramework, "WGControls", 2, 0, "WGFilteredTreeModel");
	registerType<WGListModel>(uiFramework, "WGControls", 2, 0, "WGListModel");
	registerType<WGStringFilter>(uiFramework, "WGControls", 2, 0, "WGStringFilter");
	registerType<WGTokenizedStringFilter>(uiFramework, "WGControls", 2, 0, "WGTokenizedStringFilter");
	registerType<WGTreeModel>(uiFramework, "WGControls", 2, 0, "WGTreeModel");
	registerType<WGTreeListAdapter>(uiFramework, "WGControls", 2, 0, "WGTreeListAdapter");
	registerType<ValueExtension>(uiFramework, "WGControls", 2, 0, "ValueExtension");
	// New from 2.0
	registerType<ColumnExtension>(uiFramework, "WGControls", 2, 0, "ColumnExtension");
	registerType<ComponentExtension>(uiFramework, "WGControls", 2, 0, "ComponentExtension");
	registerType<ImageExtension>(uiFramework, "WGControls", 2, 0, "ImageExtension");
	registerType<ListExtension>(uiFramework, "WGControls", 2, 0, "ListExtension");
	registerType<TableExtension>(uiFramework, "WGControls", 2, 0, "TableExtension");
	registerType<TreeExtension>(uiFramework, "WGControls", 2, 0, "TreeExtension");
	registerType<SpreadsheetExtension>(uiFramework, "WGControls", 2, 0, "SpreadsheetExtension");
	registerType<WGItemView>(uiFramework, "WGControls", 2, 0, "WGItemView");
	registerType<WGColumnLayoutProxy>(uiFramework, "WGControls", 2, 0, "WGColumnLayoutProxy");
	registerType<WGSortFilterProxy>(uiFramework, "WGControls", 2, 0, "WGSortFilterProxy");
	registerType<WGTransposeProxy>(uiFramework, "WGControls", 2, 0, "WGTransposeProxy");
	registerType<WGMergeProxy>(uiFramework, "WGControls", 2, 0, "WGMergeProxy");
	registerType<WGSubProxy>(uiFramework, "WGControls", 2, 0, "WGSubProxy");

	registerType<ButtonsDefinitionExtension>(uiFramework, "WGControls", 1, 0, "ButtonsDefinitionExtension");
	registerType<ButtonsModelExtension>(uiFramework, "WGControls", 1, 0, "ButtonsModelExtension");
}

//==============================================================================
/*static */ void SharedControls::initDefs(IDefinitionManager& definitionManager)
{
	definitionManager.registerDefinition<TypeClassDefinition<IAssetBrowserModel>>();
	definitionManager.registerDefinition<TypeClassDefinition<IAssetBrowserViewModel>>();
	definitionManager.registerDefinition<TypeClassDefinition<IAssetBrowserEventModel>>();
	definitionManager.registerDefinition<TypeClassDefinition<AssetBrowser20::IAssetBrowserModel>>();
	definitionManager.registerDefinition<TypeClassDefinition<IActiveFiltersModel>>();
	definitionManager.registerDefinition<TypeClassDefinition<ActiveFilterTerm>>();
	definitionManager.registerDefinition<TypeClassDefinition<SavedActiveFilter>>();
	definitionManager.registerDefinition<TypeClassDefinition<IBreadcrumbsModel>>();
	definitionManager.registerDefinition<TypeClassDefinition<BaseBreadcrumbItem>>();
	definitionManager.registerDefinition<TypeClassDefinition<DialogModel>>();
}
} // end namespace wgt
