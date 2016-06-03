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
#include "models/extensions/asset_item_extension.hpp"
#include "models/extensions/column_extension.hpp"
#include "models/extensions/column_extension_old.hpp"
#include "models/extensions/component_extension.hpp"
#include "models/extensions/component_extension_old.hpp"
#include "models/extensions/list_extension.hpp"
#include "models/extensions/selection_extension.hpp"
#include "models/extensions/thumbnail_extension.hpp"
#include "models/extensions/tree_extension.hpp"
#include "models/extensions/tree_extension_old.hpp"
#include "models/extensions/value_extension.hpp"
#include "models/extensions/image_extension.hpp"
#include "models/extensions/header_footer_text_extension.hpp"
#include "models/wg_list_model.hpp"
#include "models/wg_tree_model.hpp"
#include "models/wg_tree_list_adapter.hpp"
#include "helpers/selection_helper.hpp"
#include "core_data_model/asset_browser/i_asset_browser_model.hpp"
#include "core_data_model/asset_browser/i_asset_browser_view_model.hpp"
#include "core_data_model/asset_browser/i_asset_browser_event_model.hpp"
#include "core_data_model/asset_browser/asset_browser_breadcrumbs_model.hpp"
#include "core_data_model/asset_browser/metadata/i_asset_browser_model.mpp"
#include "core_data_model/asset_browser/metadata/i_asset_browser_event_model.mpp"
#include "core_data_model/asset_browser/metadata/i_asset_browser_view_model.mpp"
#include "core_data_model/i_active_filters_model.hpp"
#include "core_data_model/i_breadcrumbs_model.hpp"
#include "core_data_model/metadata/i_active_filters_model.mpp"
#include "core_data_model/metadata/i_breadcrumbs_model.mpp"
#include <QtQuick>

#include "core_reflection/i_definition_manager.hpp"
#include "core_reflection/reflection_macros.hpp"


namespace wgt
{
//==============================================================================
/*static */void SharedControls::init()
{
	// DEPRECATED BW Controls
	qmlRegisterType< DataChangeNotifier >( "BWControls", 1, 0, "BWDataChangeNotifier" );
	qmlRegisterType< SequenceListAdapter >("BWControls", 1, 0, "SequenceList" );
	qmlRegisterType< WGCopyController >( "BWControls", 1, 0, "BWCopyable" );
	qmlRegisterType< WGFileDialogQI >( "BWControls", 1, 0, "BWFileDialogQI" );

	// WG Controls 1.x
	qmlRegisterType< AssetItemExtension>( "WGControls", 1, 0, "AssetItemExtension" );
	qmlRegisterType< ColumnExtensionOld>( "WGControls", 1, 0, "ColumnExtension" );
	qmlRegisterType< ComponentExtensionOld>( "WGControls", 1, 0, "ComponentExtension" );
	qmlRegisterType< DataChangeNotifier >( "WGControls", 1, 0, "WGDataChangeNotifier" );
	qmlRegisterType< HeaderFooterTextExtension>( "WGControls", 1, 0, "HeaderFooterTextExtension" );
	qmlRegisterType< SelectionExtension>( "WGControls", 1, 0, "SelectionExtension" );
	qmlRegisterType< SelectionHelper >( "WGControls", 1, 0, "WGSelectionHelper" );
	qmlRegisterType< SequenceListAdapter >("WGControls", 1, 0, "WGSequenceList" );
	qmlRegisterType< ThumbnailExtension>( "WGControls", 1, 0, "ThumbnailExtension" );
	qmlRegisterType< TreeExtensionOld>( "WGControls", 1, 0, "TreeExtension" );
	qmlRegisterType< WGAction >( "WGControls", 1, 0, "WGAction" );
	qmlRegisterType< WGCopyController >( "WGControls", 1, 0, "WGCopyController" );
	qmlRegisterType< WGContextMenu >( "WGControls", 1, 0, "WGContextMenu" );
	qmlRegisterType< WGFileDialogQI >( "WGControls", 1, 0, "WGFileDialogQI" );
	qmlRegisterType< WGFilter>( "WGControls", 1, 0, "WGFilter" );
	qmlRegisterType< WGFilteredListModel >( "WGControls", 1, 0, "WGFilteredListModel" );
	qmlRegisterType< WGFilteredTreeModel >( "WGControls", 1, 0, "WGFilteredTreeModel" );
	qmlRegisterType< WGListModel >( "WGControls", 1, 0, "WGListModel" );
	qmlRegisterType< WGStringFilter>( "WGControls", 1, 0, "WGStringFilter" );
	qmlRegisterType< WGTokenizedStringFilter>( "WGControls", 1, 0, "WGTokenizedStringFilter" );
	qmlRegisterType< WGTreeModel >( "WGControls", 1, 0, "WGTreeModel" );
	qmlRegisterType< WGTreeListAdapter>( "WGControls", 1, 0, "WGTreeListAdapter" );
	qmlRegisterType< ValueExtension>( "WGControls", 1, 0, "ValueExtension" );

	qmlRegisterType< WGCopyController, 1 > ( "WGCopyController", 1, 1, "WGCopyController" );
	// WG Controls 2.x
	// Exist from 1.0
	qmlRegisterType< AssetItemExtension>( "WGControls", 2, 0, "AssetItemExtension" );
	qmlRegisterType< ColumnExtensionOld>( "WGControls", 2, 0, "ColumnExtension" );
	qmlRegisterType< ComponentExtensionOld>( "WGControls", 2, 0, "ComponentExtension" );
	qmlRegisterType< DataChangeNotifier >( "WGControls", 2, 0, "WGDataChangeNotifier" );
	qmlRegisterType< HeaderFooterTextExtension>( "WGControls", 2, 0, "HeaderFooterTextExtension" );
	qmlRegisterType< SelectionExtension>( "WGControls", 2, 0, "SelectionExtension" );
	qmlRegisterType< SelectionHelper >( "WGControls", 2, 0, "WGSelectionHelper" );
	qmlRegisterType< SequenceListAdapter >("WGControls", 2, 0, "WGSequenceList" );
	qmlRegisterType< ThumbnailExtension>( "WGControls", 2, 0, "ThumbnailExtension" );
	qmlRegisterType< TreeExtensionOld>( "WGControls", 2, 0, "TreeExtension" );
	qmlRegisterType< WGAction >( "WGControls", 2, 0, "WGAction" );
	qmlRegisterType< WGCopyController >( "WGControls", 2, 0, "WGCopyController" );
	qmlRegisterType< WGContextMenu >( "WGControls", 2, 0, "WGContextMenu" );
	qmlRegisterType< WGFileDialogQI >( "WGControls", 2, 0, "WGFileDialogQI" );
	qmlRegisterType< WGFilter>( "WGControls", 2, 0, "WGFilter" );
	qmlRegisterType< WGFilteredListModel >( "WGControls", 2, 0, "WGFilteredListModel" );
	qmlRegisterType< WGFilteredTreeModel >( "WGControls", 2, 0, "WGFilteredTreeModel" );
	qmlRegisterType< WGListModel >( "WGControls", 2, 0, "WGListModel" );
	qmlRegisterType< WGStringFilter>( "WGControls", 2, 0, "WGStringFilter" );
	qmlRegisterType< WGTokenizedStringFilter>( "WGControls", 2, 0, "WGTokenizedStringFilter" );
	qmlRegisterType< WGTreeModel >( "WGControls", 2, 0, "WGTreeModel" );
	qmlRegisterType< WGTreeListAdapter>( "WGControls", 2, 0, "WGTreeListAdapter" );
	qmlRegisterType< ValueExtension>( "WGControls", 2, 0, "ValueExtension" );
	// New from 2.0
	qmlRegisterType< ColumnExtension >( "WGControls", 2, 0, "ColumnExtension" );
	qmlRegisterType< ComponentExtension >( "WGControls", 2, 0, "ComponentExtension" );
	qmlRegisterType< ImageExtension >( "WGControls", 2, 0, "ImageExtension" );
	qmlRegisterType< ListExtension >( "WGControls", 2, 0, "ListExtension" );
	qmlRegisterType< TreeExtension >( "WGControls", 2, 0, "TreeExtension" );
	qmlRegisterType< WGItemView >( "WGControls", 2, 0, "WGItemView" );

}


//==============================================================================
/*static */void SharedControls::initDefs(
	IDefinitionManager & definitionManager )
{
	definitionManager.registerDefinition<TypeClassDefinition< IAssetBrowserModel >>();
	definitionManager.registerDefinition<TypeClassDefinition< IAssetBrowserViewModel >>();
	definitionManager.registerDefinition<TypeClassDefinition< IAssetBrowserEventModel >>();
	definitionManager.registerDefinition<TypeClassDefinition< IActiveFiltersModel >>();
	definitionManager.registerDefinition<TypeClassDefinition< ActiveFilterTerm >>();
	definitionManager.registerDefinition<TypeClassDefinition< SavedActiveFilter >>();
	definitionManager.registerDefinition<TypeClassDefinition< IBreadcrumbsModel >>();
	definitionManager.registerDefinition<TypeClassDefinition< BaseBreadcrumbItem >>();
}
} // end namespace wgt
