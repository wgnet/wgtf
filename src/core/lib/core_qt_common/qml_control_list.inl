#include "controls/wg_context_menu.hpp"
#include "controls/wg_action.hpp"
#include "controls/wg_item_view.hpp"
#include "controls/connection_curve.hpp"
#include "helpers/wg_filtered_list_model.hpp"
#include "helpers/wg_filtered_tree_model.hpp"
#include "helpers/wg_filter.hpp"
#include "helpers/wg_string_filter.hpp"
#include "helpers/wg_tokenized_string_filter.hpp"
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
#include "models/wg_asset_filter_proxy.hpp"
#include "models/wg_column_layout_proxy.hpp"
#include "models/wg_sort_filter_proxy.hpp"
#include "models/wg_transpose_proxy.hpp"
#include "models/wg_merge_proxy.hpp"
#include "models/wg_sub_proxy.hpp"
#include "models/wg_range_proxy.hpp"
#include "models/wg_multi_edit_proxy.hpp"
#include "models/wg_fast_filter_proxy.hpp"
#include "models/wg_flatten_proxy.hpp"
#include "helpers/selection_helper.hpp"
#include "core_data_model/i_active_filters_model.hpp"
#include "core_data_model/dialog/dialog_model.hpp"
#include "core_data_model/dialog/reflected_dialog_model.hpp"
#include "models/qml_model_data.hpp"
#include "models/qml_model_row.hpp"
#include "models/qml_list_model.hpp"
#include "models/qml_table_model.hpp"
#include "models/qml_tree_model.hpp"
#include "qt_spinbox_validator.hpp"
#include "filters/folder_filter_object.hpp"
#include "filters/invert_filter_object.hpp"
#include "filters/search_filter_object.hpp"

#include <QQmlEngine>

namespace wgt
{
static void registerQmlTypes()
{
	qmlRegisterType<ColumnExtensionOld>( "WGControls", 1, 0, "ColumnExtension" );
	qmlRegisterType<ComponentExtensionOld>( "WGControls", 1, 0, "ComponentExtension" );
	qmlRegisterType<HeaderFooterTextExtension>( "WGControls", 1, 0, "HeaderFooterTextExtension" );
	qmlRegisterType<SelectionExtension>( "WGControls", 1, 0, "SelectionExtension" );
	qmlRegisterType<SelectionHelper>( "WGControls", 1, 0, "WGSelectionHelper" );
	qmlRegisterType<ThumbnailExtension>( "WGControls", 1, 0, "ThumbnailExtension" );
	qmlRegisterType<TreeExtensionOld>( "WGControls", 1, 0, "TreeExtension" );
	qmlRegisterType<WGAction>( "WGControls", 1, 0, "WGAction" );
	qmlRegisterType<WGContextMenu>( "WGControls", 1, 0, "WGContextMenu" );
	qmlRegisterType<WGFilter>( "WGControls", 1, 0, "WGFilter" );
	qmlRegisterType<WGFilteredListModel>( "WGControls", 1, 0, "WGFilteredListModel" );
	qmlRegisterType<WGFilteredTreeModel>( "WGControls", 1, 0, "WGFilteredTreeModel" );
	qmlRegisterType<WGListModel>( "WGControls", 1, 0, "WGListModel" );
	qmlRegisterType<WGStringFilter>( "WGControls", 1, 0, "WGStringFilter" );
	qmlRegisterType<WGTokenizedStringFilter>( "WGControls", 1, 0, "WGTokenizedStringFilter" );
	qmlRegisterType<WGTreeModel>( "WGControls", 1, 0, "WGTreeModel" );
	qmlRegisterType<WGTreeListAdapter>( "WGControls", 1, 0, "WGTreeListAdapter" );
	qmlRegisterType<ValueExtension>( "WGControls", 1, 0, "ValueExtension" );
	qmlRegisterType<WGColumnLayoutProxy>( "WGControls", 1, 0, "WGColumnLayoutProxy" );

	// WG Controls 2.x
	// Exist from 1.0
	qmlRegisterType<SelectionHelper>( "WGControls", 2, 0, "WGSelectionHelper" );
	qmlRegisterType<WGAction>( "WGControls", 2, 0, "WGAction" );
	qmlRegisterType<WGContextMenu>( "WGControls", 2, 0, "WGContextMenu" );
	qmlRegisterType<QmlModelData>("WGControls", 2, 0, "WGModelData");
	qmlRegisterType<QmlModelRow>("WGControls", 2, 0, "WGModelRow");
	qmlRegisterType<QmlListModel>("WGControls", 2, 0, "WGListModel");
	qmlRegisterType<QmlTableModel>("WGControls", 2, 0, "WGTableModel");
	qmlRegisterType<QmlTreeModel>("WGControls", 2, 0, "WGTreeModel");
	// New from 2.0
	qmlRegisterType<WGItemView>( "WGControls", 2, 0, "WGItemView" );
	qmlRegisterType<WGColumnLayoutProxy>( "WGControls", 2, 0, "WGColumnLayoutProxy" );
	qmlRegisterType<WGSortFilterProxy>( "WGControls", 2, 0, "WGSortFilterProxy" );
	qmlRegisterType<WGSortFilterProxy>("WGControls", 2, 0, "SortFilterResult");
	qmlRegisterType<WGTransposeProxy>( "WGControls", 2, 0, "WGTransposeProxy" );
	qmlRegisterType<WGMergeProxy>( "WGControls", 2, 0, "WGMergeProxy" );
	qmlRegisterType<WGSubProxy>( "WGControls", 2, 0, "WGSubProxy" );
	qmlRegisterType<WGRangeProxy>( "WGControls", 2, 0, "WGRangeProxy" );
	qmlRegisterType<WGMultiEditProxy>("WGControls", 2, 0, "WGMultiEditProxy");
	qmlRegisterType<WGAssetFilterProxy>("WGControls", 2, 0, "WGAssetFilterProxy");
	qmlRegisterType<WGFastFilterProxy>("WGControls", 2, 0, "WGFastFilterProxy");
	qmlRegisterType<WGFlattenProxy>("WGControls", 2, 0, "WGFlattenProxy");

	qmlRegisterType<ButtonsDefinitionExtension>( "WGControls", 1, 0, "ButtonsDefinitionExtension" );
	qmlRegisterType<ButtonsModelExtension>( "WGControls", 1, 0, "ButtonsModelExtension" );

	qmlRegisterType<WGSpinBoxValidator>("WGControls", 2, 0, "WGSpinBoxValidator");

	qmlRegisterType<FolderFilterObject>("WGControls", 2, 0, "FolderFilterRole");
	qmlRegisterType<FolderFilterObject>("WGControls", 2, 0, "FolderFilterObject");
	qmlRegisterType<InvertFilterObject>("WGControls", 2, 0, "InvertFilterObject");
	qmlRegisterType<SearchFilterObject>("WGControls", 2, 0, "SearchFilterRole");
	qmlRegisterType<SearchFilterObject>("WGControls", 2, 0, "SearchFilterObject");

#ifndef QT_SHARABLE_LIB
	qmlRegisterType<ConnectionCurve>( "CustomConnection", 1, 0, "ConnectionCurve" );
#endif
}
}