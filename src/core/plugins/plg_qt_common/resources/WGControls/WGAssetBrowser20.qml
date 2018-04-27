import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import QtQml.Models 2.2

import WGControls 2.0
import WGControls.Private 2.0
import WGControls.Views 2.0
import WGControls.Layouts 2.0
import WGControls.Styles 2.0
import WGControls.Global 2.0

import QtQml.Models 2.2 as QtQmlModels

/*!
 \ingroup wgcontrols
 \brief A control used for display, browsing and interaction of assets on disc.
        It is operating off data models for file folders and the listview. The layout is based upon:
        Search for Asset Browser Qt Prototype on the Wargaming Confluence
Example:
\code{.js}
WGAssetBrowser {
    id: testAssetBrowserControl
    model: fileSystemModel
}
\endcode
*/

//ToDo: Better naming of ObjectName for the thumbnails and repeated elements within assetbrowser using itemData
/*ToDo: It might be possilbe to reduce the number of calls to the following functions;
checkState(), checkPreviewState(), checkValidWidthAndHeight(), checkPreviewValidWidthAndHeight() */

Rectangle {
    id: root
    objectName: "WGAssetBrowser"
    WGComponent { type: "WGAssetBrowser20" }

    color: palette.mainWindowColor

    /*! This property determines the default size of the icons in the treeview.*/
    property int treeViewIconSize: 16

    /*! This property determines the default size of the icons in the listview. A value < 32 will default contents to the list view */
    property int iconSize: 16

    /*! This property determines the pixel size of the icon file to choose */
    property string iconFileSize: (iconSize <= 24 ? "16" : (iconSize <= 64 ? "48" : ""))

    /*! This property determines the size of the label of each icon */
    property int iconLabelSize: iconSize > 32 ? 9 : 7

    /*!  This property indicates if the asset browser is showing a grid (true) or a list view (false) */
    readonly property bool showIcons: iconSize >= 32

    /*!  This property indicates if files that support thumbnails show them or just use icons */
    property bool showThumbnails: true

    /*!  This property indicates if labels are shown. Only applies to icon view. */
    property bool showLabels: true

    /*!  This property indicates if the Preview Panel is shown. */
    property bool showPreview: true

    /*!  This property indicates if overlay status icons are show on top of regular icons. */
    property bool showOverlayIcons: true

    /*!  This property indicates if the following buttons/features are shown. */
    property bool showRefreshButton: showTopBar
    property bool showHiddenButton: showTopBar
    property bool showLabelButton: showTopBar
    property bool showPreviewButton: showTopBar
    property bool showThumbnailsButton: showTopBar
    property bool showIconsButton: showTopBar
    property bool showOverlayIconsButton: showTopBar
    property bool showHistoryButton: showTopBar
    property bool showForwardButton: showTopBar
    property bool showBackButton: showTopBar
    property bool showBreadcrumbs: showTopBar
    property bool showOrientationButton: showTopBar
    property bool showHideFoldersButton: showTopBar
    property bool showHideContentsButton: showTopBar

    /*!  Setting this to false will hide all the controls in the top bar */
    property bool showTopBar: true

    /*!  This property indicates if hidden files and folders are shown. */
    property bool showHidden: false

    /*!  Breadcrumbs font size */
    property var breadCrumbsFontSize: 9

    /*!  Enums for browserLayout state  */
    readonly property var horizontalLayout: "HORIZONTAL"
    readonly property var verticalLayout: "VERTICAL"
    readonly property var autoLayout: "AUTO"
    property var defaultLayoutState: horizontalLayout

    /*! Indicates whether the source model has a concept of folders and contents */
    property bool haveFoldersAndContent: true

    /*! For enabling/disabling the contents frame */
    property bool useContentsFrame: true

    /*! For enabling/disabling the contents frame */
    property bool useFolderFrame: true

    /*!  This property determines the orientation of the tree and itemview */
    property var assetViewOrientation: null

    /*!  This property determines the orientation of the preview frame */
    property var previewViewOrientation: null

    /*!  This property determines if the orientation of the browser will change based on the window size. */
    property bool autoOrientation: assetViewOrientation == autoLayout

    /*!  This property determines if the orientation of the preview browser will change based on the window size. */
    property bool previewAutoOrientation: previewViewOrientation == autoLayout

    /*! Paths for the right mouse click context menus */
    property var folderContextMenuPath: "WGAssetBrowser20FolderMenu"
    property var assetContextMenuPath: "WGAssetBrowser20AssetMenu"

    property alias folderContextObject: folderContextMenu.contextObject
    property alias assetContextObject: assetContextMenu.contextObject

    property var model: null

    /*! Filtering for what assets are considered visible */
    property var nameFilters: null
    property var nameFiltersLabel: "File Type"
    property var selectedNameFilter: ""
    property var nameFilter: WGPath.extractFilters(selectedNameFilter);

    /*!  This property used to access folder tree's active filters */
    property alias folderFilterString: activeFilters.filterString

    /*! Override this to determine what the folder panel will display, or null to show all */
    property alias folderFilterObject: folderModel.filterObject

    /*! Override this to determine what role the search filter will use for comparison */
    property var searchFilterRole: SearchFilterRole.FullPath

    property string collapsedTreeNodeIconRole: ""
    property string expandedTreeNodeIconRole: ""
    property string leafTreeNodeIconRole: ""
    property string defaultCollapsedTreeNodeIconKey: ""
    property string defaultExpandedTreeNodeIconKey: ""
    property string defaultLeafTreeNodeIconKey: ""

    /*! Path to the currently selected folder */
    property string currentFolder: ""

    /*! Path to the currently selected asset */
    property var currentPath: ""

    /*! This property determines the maximum number of history items tracked during asset tree navigation
        The default value is \c 10 */
    property int maxHistoryItems: 10

    /*!  This component used to show folders */
    property Component assetFolderView: WGAssetTreeView {}

    /*!  This property used to select the first root folder when this control been created */
    property int defaultSelectedFolderIndex: -1

    property alias itemviewPreviewSplitterState: itemviewPreviewSplitter.state

    property alias folderTree: folderLoader.item
    property alias contentView: assetView.item
    property var contentViewModel: searchResultsModel.filterText != "" ? searchResultsModel : contentsModel
    property var iconViewSelectionModel: contentViewModel == searchResultsModel ? iconViewSearchResultsSelectionModel : iconViewContentsSelectionModel
    property var listViewSelectionModel: contentViewModel == searchResultsModel ? listViewSearchResultsSelectionModel : listViewContentsSelectionModel

    property var supportMultiViewSelect: false




    /*!  SelectionModel shared between the two item views */
    property var contentsSelectionModel: ItemSelectionModel {
        model: root.model
        property var supportMultiViewSelect: root.supportMultiViewSelect
    }
    property var iconViewSearchResultsSelectionModel: QtObject {
        property var itemSelectionModel: contentsSelectionModel
        property var mapToSource: function(searchResultsModelIndex) {
            var searchModelIndex = searchResultsModel.mapToSource(searchResultsModelIndex);
            var flattenedContentsModelIndex = searchModel.mapToSource(searchModelIndex);
            var contentsModelIndex = flattenedContentsModel.mapToSource(flattenedContentsModelIndex);
            if (contentsModelIndex.row < 0) {
                return null;
            }
            var filteredModelIndex = contentsModel.mapToSource(contentsModelIndex);
            var assetsModelIndex = filteredModel.mapToSource(filteredModelIndex);
            var sourceIndex = assetsModel.mapToSource(assetsModelIndex);
            return sourceIndex;
        }
        property var mapFromSource: function(sourceIndex) {
            var assetsModelIndex = assetsModel.mapFromSource(sourceIndex);
            var filteredModelIndex = filteredModel.mapFromSource(assetsModelIndex);
            if (contentsModel.sourceModel == null) {
                return null;
            }
            var parentIndex = filteredModelIndex.parent;
            while (parentIndex != contentsModel.sourceParent) {
                var grandParentIndex = parentIndex.parent;
                if (parentIndex == grandParentIndex) {
                    return null;
                }
                parentIndex = grandParentIndex;
            }
            var contentsModelIndex = contentsModel.mapFromSource(filteredModelIndex);
            var flattenedContentsModelIndex = flattenedContentsModel.mapFromSource(contentsModelIndex);
            var searchModelIndex = searchModel.mapFromSource(flattenedContentsModelIndex);
            var searchResultsModelIndex = searchResultsModel.mapFromSource(searchModelIndex);
            return searchResultsModelIndex;
        }
        property var mapSelectionFromSource: function(selection) {
            var assetsModelSelection = assetsModel.mapSelectionFromSource(selection);
            var filteredModelSelection = filteredModel.mapSelectionFromSource(assetsModelSelection);
            var contentsModelSelection = contentsModel.mapSelectionFromSource(filteredModelSelection);
            var flattenedContentsModelSelection = flattenedContentsModel.mapSelectionFromSource(contentsModelSelection);
            var searchModelSelection = searchModel.mapSelectionFromSource(flattenedContentsModelSelection);
            var searchResultsModelSelection = searchResultsModel.mapSelectionFromSource(searchModelSelection);
            return searchResultsModelSelection;
        }
    }
    property var iconViewContentsSelectionModel: QtObject {
        property var itemSelectionModel: contentsSelectionModel
        property var mapToSource: function(contentsModelIndex) {
            if (contentsModelIndex.row < 0) {
                return null;
            }
            var filteredModelIndex = contentsModel.mapToSource(contentsModelIndex);
            var assetsModelIndex = filteredModel.mapToSource(filteredModelIndex);
            var sourceIndex = assetsModel.mapToSource(assetsModelIndex);
            return sourceIndex;
        }
        property var mapFromSource: function(sourceIndex) {
            var assetsModelIndex = assetsModel.mapFromSource(sourceIndex);
            var filteredModelIndex = filteredModel.mapFromSource(assetsModelIndex);
            if (filteredModelIndex.parent != contentsModel.sourceParent) {
                return null;
            }
            var contentsModelIndex = contentsModel.mapFromSource(filteredModelIndex);
            return contentsModelIndex;
        }
        property var mapSelectionFromSource: function(selection) {
            var assetsModelSelection = assetsModel.mapSelectionFromSource(selection);
            var filteredModelSelection = filteredModel.mapSelectionFromSource(assetsModelSelection);
            var contentsModelSelection = contentsModel.mapSelectionFromSource(filteredModelSelection);
            return contentsModelSelection;
        }
    }
    property var listViewSearchResultsSelectionModel: QtObject {
        property var itemSelectionModel: contentsSelectionModel
        property var mapToSource: function(proxyIndex) {
            var searchResultsModelIndex = contentView.proxyModel.mapToSource(proxyIndex);
            return iconViewSearchResultsSelectionModel.mapToSource(searchResultsModelIndex);
        }
        property var mapFromSource: function(sourceIndex) {
            var searchResultsModelIndex = iconViewSearchResultsSelectionModel.mapFromSource(sourceIndex);
            var proxyIndex = contentView.proxyModel.mapFromSource(searchResultsModelIndex);
            return proxyIndex;
        }
        property var mapSelectionFromSource: function(selection) {
            var searchResultsModelSelection = iconViewSearchResultsSelectionModel.mapSelectionFromSource(selection);
            var proxySelection = contentView.proxyModel.mapSelectionFromSource(searchResultsModelSelection);
            return proxySelection;
        }
    }
    property var listViewContentsSelectionModel: QtObject {
        property var itemSelectionModel: contentsSelectionModel
        property var mapToSource: function(proxyIndex) {
            var contentsModelIndex = contentView.proxyModel.mapToSource(proxyIndex);
            return iconViewContentsSelectionModel.mapToSource(contentsModelIndex);
        }
        property var mapFromSource: function(sourceIndex) {
            var contentsModelIndex = iconViewContentsSelectionModel.mapFromSource(sourceIndex);
            var proxyIndex = contentView.proxyModel.mapFromSource(contentsModelIndex);
            return proxyIndex;
        }
        property var mapSelectionFromSource: function(selection) {
            var contentsModelSelection = iconViewContentsSelectionModel.mapSelectionFromSource(selection);
            var proxySelection = contentView.proxyModel.mapSelectionFromSource(contentsModelSelection);
            return proxySelection;
        }
    }

    /*!  This component used to show a grid of assets with icons/thumbnails */
    property Component assetIconView: WGAssetIconView {}

    /*!  This component used to show a list of assets with icons/thumbnails */
    property Component assetListView: WGAssetListView {}

    /*!  This component used to show thumbnail or asset previews */
    property Component previewPanel: WGPreviewWindow {}

    /*!  This property used to access content view's search text */
    property alias contentViewSearchText: searchEdit.text

    /*!  This property used to access content view's asset preview */
    property var currentPreview: null

    /*! Override this to determine how to get the value from an item */
    property var getItemValue: function(item) {
        if(item == null || typeof(item) == "undefined") {
            return "";
        }
        var display = item.display;
        if(typeof(display) != "undefined") {
            return display;
        }
        var value = item.value;
        if(typeof(value) != "undefined") {
            return value;
        }
        return "";
    }

    /*! Override this to determine how to get the full path for an item */
    property var getFullPath: function(item) {
        if(item == null || typeof(item) == "undefined") {
            return "";
        }
        var fullPath = item.fullPath;
        if(typeof(fullPath) != "undefined") {
            return fullPath;
        }
        return "";
    }

    function getSelectedFolderItems() {
        var model = folderTree.view.extendedModel;
        var selectionModel = folderTree.view.selectionModel;
        return internal.getSelectedItems(model, selectionModel);
    }

    function getSelectedContentItems() {
        var model = root.model;
        var selectionModel = contentsSelectionModel;
        return internal.getSelectedItems(model, selectionModel);
    }

    /*! This function is called by the preview window to automatically change layout*/
    function callCheckPreviewWidthToggleOrientation() {
        itemviewPreviewSplitter.checkPreviewWidthToggleOrientation()
    }

    function addToFilter(str) {
        activeFilters.addFilter(str);
    }

    function clearFilters() {
        activeFilters.clearFilters();
    }


    /*! This function is used to get icon of item in asset views*/
    function getIconSource(item, hasChildren, imageSize) {
        var validItemData = typeof item != "undefined" && item != null;
        var hasDecorationRole = validItemData && typeof(item.decoration) != "undefined";
        var expanded = hasChildren && typeof(item.expanded) != "undefined" && item.expanded;
        var iconData = "";
        if(!validItemData)
        {
            return iconData;
        }
        if(hasChildren && expanded)
        {
            // it's an expanded tree node
            iconData = expandedTreeNodeIconRole != "" ? item[root.expandedTreeNodeIconRole] : hasDecorationRole ? item.decoration : ""
            var imageUrl = getIconUrlFromImageProvider(iconData);
            if(imageUrl == "")
            {
                //if image can be found in image provider
                iconData = defaultExpandedTreeNodeIconKey;
            }
        }
        else if(hasChildren)
        {
            // it's a collapsed tree node
            iconData = collapsedTreeNodeIconRole != "" ? item[root.collapsedTreeNodeIconRole] : hasDecorationRole ? item.decoration : ""
            var imageUrl = getIconUrlFromImageProvider(iconData);
            if(imageUrl == "")
            {
                //if image can be found in image provider
                iconData = defaultCollapsedTreeNodeIconKey;
            }
        }
        else
        {
            // it's a leaf node
            iconData = leafTreeNodeIconRole != "" ? item[root.leafTreeNodeIconRole] : hasDecorationRole ? item.decoration : ""
            var imageUrl = getIconUrlFromImageProvider(iconData);
            if(imageUrl == "")
            {
                //if image can be found in image provider
                iconData = defaultLeafTreeNodeIconKey;
            }
        }

        if (typeof imageSize != 'undefined')
        {
            iconData += imageSize
        }

        return iconData;
    }

    /*! This function is used to get thumbnail of item in asset icon views*/
    function getThumbnailSource(item) {
        var validItemData = typeof item != "undefined" && item != null;
        var hasThumbnailRole = validItemData && typeof(item.thumbnail) != "undefined";
        var thumbnailData = "";
        if(!hasThumbnailRole)
        {
            return thumbnailData;
        }
        thumbnailData = "image://QtThumbnailProvider/" + item.thumbnail
        return thumbnailData;
    }

    /*! Clears the current selection */
    function clearSelection() {
        if (folderTree) {
            folderTree.clearSelection();
            folderTree.updateScrollPosition();
        }

        if(contentView) {
            contentView.clearSelection();
            contentView.updateScrollPosition();
        }
    }

    /*! Selects the given path if it's valid*/
    function selectPath(path) {
        var pathN = WGPath.normalisePath(path);
        var folderPath = pathN.slice(0, pathN.lastIndexOf("\\") + 1);
        if (!folderTree || !folderTree.selectPath(folderPath)) {
            return false;
        }

        if(!contentView || !contentView.selectPath(pathN)) {
            return false;
        }

        folderTree.updateScrollPosition();
        contentView.updateScrollPosition();
        resetTimer.stop();
        return true;
    }

    /*! Selects the given folder if it's valid */
    function selectFolder(path) {
        if (!folderTree || !folderTree.selectPath(path)) {
            return false;
        }

        folderTree.updateScrollPosition();
        resetTimer.stop();
        return true;
    }

    function selectDefaultFolder() {
        if (!folderTree) {
            return false;
        }

        var rootIndex = folderTree.model.index(defaultSelectedFolderIndex, 0);
        var rootItem = folderTree.indexToItem(rootIndex);
        return selectFolder(root.getFullPath(rootItem));
    }

    /* Toggle the orientation of the assetSplitter if in Auto layout mode */
    function checkWidthToggleOrientation(){
        if (autoOrientation)
        {
            if (width < height * 0.8 && assetSplitter.state == "HORIZONTAL")
            {
                assetSplitter.state = "VERTICAL"
            }
            else if (width > height * 1.2 && assetSplitter.state == "VERTICAL")
            {
                assetSplitter.state = "HORIZONTAL"
            }
        }
    }

    /* Following a change to useContentsFrame, reset firstSplitViewLayout.width/height to its preferredWidth/Height*/
    // ToDo: Note that height is never set, yet it is working correctly. Potential for simplification here.
    onUseContentsFrameChanged: {
        if ((useContentsFrame || internal.useContentsFrame) && assetSplitter.orientation == Qt.Horizontal &&
                firstSplitViewLayout.width != firstSplitViewLayout.Layout.preferredWidth)
        {
            firstSplitViewLayout.width = firstSplitViewLayout.Layout.preferredWidth
        }
        checkValidWidthAndHeight()
    }

    /*! Ensures the assetSplitter starts with a valid state so auto layout works */
    function checkState() {
        if (assetSplitter.state == "") {
            if (assetSplitter.orientation == Qt.Horizontal) {
                assetSplitter.state = horizontalLayout
            }
            else {
                assetSplitter.state = verticalLayout
            }
        }
    }

    /*! Ensures the itemviewPreviewSplitter starts with a valid state so auto layout works */
    function checkPreviewState() {
        if (itemviewPreviewSplitter.state == "") {
            if (itemviewPreviewSplitter.orientation == Qt.Horizontal) {
                itemviewPreviewSplitter.state = horizontalLayout
            }
            else {
                itemviewPreviewSplitter.state = verticalLayout
            }
        }
    }

    /*! Ensure the firstSplitViewLayout does not exceed the width/height of the assetSplitter which can occur when stretched
    over two monitors in previous session */
    function checkValidWidthAndHeight(){
        // If the firstSplitViewLayout is wider than the assetSplitter then make it fit
        if (assetSplitter.orientation == Qt.Horizontal && internal.useFolderFrame && internal.useContentsFrame &&
                firstSplitViewLayout.width > (assetSplitter.width - secondSplitViewLayout.Layout.minimumWidth - internal.splitViewSeparatorWidth))
        {
            firstSplitViewLayout.width = assetSplitter.width - secondSplitViewLayout.Layout.minimumWidth - internal.splitViewSeparatorWidth
        }
        /* Checks there's enough room to make width = preferredWidth.
        Required because the above if statement is triggered on construction due to irregular startup widths */
        // ToDo: When in auto mode this will be triggered on a resize. Causing the view to grab a large amount of the realestate. Not sure if want to fix.
        if (assetSplitter.orientation == Qt.Horizontal && internal.useFolderFrame && internal.useContentsFrame &&
                firstSplitViewLayout.width != firstSplitViewLayout.Layout.preferredWidth &&
                firstSplitViewLayout.Layout.preferredWidth < (assetSplitter.width - secondSplitViewLayout.Layout.minimumWidth -
                internal.splitViewSeparatorWidth))
        {
            firstSplitViewLayout.width = firstSplitViewLayout.Layout.preferredWidth
        }

        // If the firstSplitViewLayout is taller than the assetSplitter then make it fit
        if (assetSplitter.orientation == Qt.Vertical && internal.useFolderFrame && internal.useContentsFrame && firstSplitViewLayout.height >
                (assetSplitter.height - secondSplitViewLayout.Layout.minimumHeight - internal.splitViewSeparatorWidth))
        {
            firstSplitViewLayout.height = assetSplitter.height - secondSplitViewLayout.Layout.minimumHeight - internal.splitViewSeparatorWidth
        }
        /*! Check there's enough room to make width = preferredWidth.
        Required because the above if statement is triggered on construction due to irregular startup widths */
        if (assetSplitter.orientation == Qt.Vertical && internal.useContentsFrame &&
                firstSplitViewLayout.height != firstSplitViewLayout.Layout.preferredHeight &&
                firstSplitViewLayout.Layout.preferredHeight < (assetSplitter.height - secondSplitViewLayout.Layout.minimumHeight - internal.splitViewSeparatorWidth))
        {
            firstSplitViewLayout.height = firstSplitViewLayout.Layout.preferredHeight
        }
    }

    /* Ensure the previewHolder does not exceed the width/height of the itemviewPreviewSplitter which can occur when stretched
    over two monitors in previous session */
    function checkPreviewValidWidthAndHeight() {
        // If the previewHolder is wider than the itemviewPreviewSplitter then make it fit
        if (itemviewPreviewSplitter.orientation == Qt.Horizontal && showPreview && internal.useContentsFrame && previewHolder.width >
                (itemviewPreviewSplitter.width - assetFrame.Layout.minimumWidth - internal.splitViewSeparatorWidth))
        {
            previewHolder.width = itemviewPreviewSplitter.width - assetFrame.Layout.minimumWidth - internal.splitViewSeparatorWidth
        }
        /* Check there's enough room to make width = preferredWidth.
        Required because the above if statement is triggered on construction due to irregular startup widths */
        // ToDo: When in auto mode this will be triggered on a resize. Causing the view to grab a large amount of the realestate. Not sure if want to fix.
        if (itemviewPreviewSplitter.orientation == Qt.Horizontal && showPreview && previewHolder.width != previewHolder.Layout.preferredWidth &&
                previewHolder.Layout.preferredWidth < (itemviewPreviewSplitter.width - assetFrame.Layout.minimumWidth - internal.splitViewSeparatorWidth))
        {
            previewHolder.width = previewHolder.Layout.preferredWidth
        }

        // If the previewHolder is taller than the itemviewPreviewSplitter then make it fit
        if (itemviewPreviewSplitter.orientation == Qt.Vertical && showPreview && internal.useContentsFrame && previewHolder.height >
                (itemviewPreviewSplitter.height - assetFrame.Layout.minimumHeight - internal.splitViewSeparatorWidth))
        {
            previewHolder.height = itemviewPreviewSplitter.height - assetFrame.Layout.minimumHeight - internal.splitViewSeparatorWidth
        }
        /*! Check there's enough room to make width = preferredWidth.
        Required because the above if statement is triggered on construction due to irregular startup widths */
        if (itemviewPreviewSplitter.orientation == Qt.Vertical && showPreview && previewHolder.height != previewHolder.Layout.preferredHeight &&
                previewHolder.Layout.preferredHeight < (itemviewPreviewSplitter.height - assetFrame.Layout.minimumHeight - internal.splitViewSeparatorWidth))
        {
            previewHolder.height = previewHolder.Layout.preferredHeight
        }
    }

    signal assetAccepted(string assetPath)
    signal previewAsset(string assetPath)

    property var assetPreview: null

    onPreviewAsset: {
        if (assetPreview != null) {
            var previewData = assetPreview(assetPath);
            root.currentPreview = previewData != 'undefined' ? previewData : null;
        }
    }

    QtObject {
        id: internal
        property int __currentFolderHistoryIndex: -1
        property bool shouldTrackFolderHistory: true
        property bool useContentsFrame: root.useContentsFrame && root.haveFoldersAndContent
        property bool useFolderFrame: (root.useFolderFrame && root.haveFoldersAndContent) || !root.haveFoldersAndContent

        // Used instead of the inaccessible SplitView's handle delegate property
        property int splitViewSeparatorWidth: defaultSpacing.separatorWidth*2

        // Handles a history menu item being clicked
        function historyMenuItemClicked(index) {
            // Make sure the index is valid
            if (folderHistory.count <= index) {
                return;
            }

            __currentFolderHistoryIndex = index;

            // Don't track the folder history while navigating said history
            shouldTrackFolderHistory = false;
            var path = folderHistory.get(__currentFolderHistoryIndex).path;
            root.selectFolder(path);
            shouldTrackFolderHistory = true;
        }

        // Tells the page to navigate the history forward or backward
        // depending on what button was clicked
        function onNavigate( isForward ) {
            if (isForward) {
                if (folderHistory.count <= __currentFolderHistoryIndex + 1) {
                    return;
                }
                __currentFolderHistoryIndex += 1;
            }
            else {
                if(__currentFolderHistoryIndex <= 0) {
                    return;
                }
                __currentFolderHistoryIndex -= 1;
            }

            // Don't track the folder history while we use the navigate buttons the history
            shouldTrackFolderHistory = false;
            var path = folderHistory.get(__currentFolderHistoryIndex).path;
            root.selectFolder(path);
            shouldTrackFolderHistory = true;
        }

        function getSelectedItems(model, selectionModel) {
            var selectedIndexes = selectionModel.selectedIndexes;
            var selectedItems = model.indexesToItems(selectedIndexes);
            selectedItems.reverse();
            return selectedItems;
        }
    }

    ListModel { id: folderHistory }

    onAssetViewOrientationChanged: {
        if(assetViewOrientation != null) {
            btnAssetBrowserOrientation.state = assetViewOrientation
            if (assetViewOrientation != "AUTO" ) {
                assetSplitter.state = assetViewOrientation
            }
        }
        checkValidWidthAndHeight()
    }

    onWidthChanged: {
        checkState()
        checkWidthToggleOrientation()
    }

    onHeightChanged: {
        checkState()
        checkWidthToggleOrientation()
    }

    Component.onCompleted: {
        if (typeof viewPreference == "undefined" || viewPreference == null) {
            if(assetViewOrientation == null) {
                assetViewOrientation = defaultLayoutState
            }
            if(previewViewOrientation == null) {
                previewViewOrientation = defaultLayoutState
            }
            return;
        }

        var vAViewOrientation = viewPreference.assetViewOrientation;
        if (typeof vAViewOrientation != "undefined" && vAViewOrientation != "") {
            assetViewOrientation = vAViewOrientation;
        }
        else if(assetViewOrientation == null) {
            assetViewOrientation = defaultLayoutState
        }

        var vPViewOrientation = viewPreference.previewViewOrientation;
        if (typeof vPViewOrientation != "undefined" && vPViewOrientation != "") {
            previewViewOrientation = vPViewOrientation
        }
        else if (previewViewOrientation == null) {
            previewViewOrientation = defaultLayoutState
        }

        var vShowPreview = viewPreference.showPreview;
        if (typeof vShowPreview != "undefined" && vShowPreview != "") {
            showPreview = (vShowPreview == "true")
        }

        var vUseContentsFrame = viewPreference.useContentsFrame;
        if (typeof vUseContentsFrame != "undefined" && vUseContentsFrame != "") {
            useContentsFrame = (vUseContentsFrame  == "true")
        }

        var vShowHidden = viewPreference.showHiddenFolders;
        if (typeof vShowHidden != "undefined" && vShowHidden != "") {
            showHidden = (vShowHidden == "true")
        }

        var vVisible = viewPreference.leftFrameVisible;
        if (typeof vVisible != "undefined" && vVisible != "") {
            useFolderFrame = (vVisible == "true")
        }

        var vIconSize = viewPreference.assetIconSize;
        if (typeof vIconSize != "undefined" && vIconSize != "") {
            iconSize = vIconSize;
        }

        var vWidth = viewPreference.leftFrameWidth;
        var vHeight = viewPreference.leftFrameHeight;

        if (typeof vWidth != "undefined") {
            firstSplitViewLayout.Layout.preferredWidth = vWidth;
        }
        else { // Cant derive width from assetSplitter or root ,as their width values are incorrect at this point in time
            firstSplitViewLayout.Layout.preferredWidth = vWidth = 280
        }

        if (typeof vHeight != "undefined") {
            firstSplitViewLayout.Layout.preferredHeight = vHeight;
        }
        else { // Cant derive width from assetSplitter or root ,as their height values are incorrect at this point in time
            firstSplitViewLayout.Layout.preferredHeight = vHeight = 150;
        }

        if(assetSplitter.orientation == Qt.Horizontal) {
            firstSplitViewLayout.width = vWidth;
        }
        else {
            firstSplitViewLayout.height = vHeight;
        }

        // set itemviewPreviewSplitter
        var vPrevWidth = viewPreference.assetPreviewWidth;
        var vPrevHeight = viewPreference.assetPreviewHeight;

        if (typeof vPrevWidth != "undefined")
        {
            previewHolder.Layout.preferredWidth = vPrevWidth;
        }
        else { // Cant derive width from itemviewPreviewSplitter or root ,as their width values are incorrect at this point in time
            previewHolder.Layout.preferredWidth = vPrevWidth = 300
        }

        if (typeof vPrevHeight != "undefined")
        {
            previewHolder.Layout.preferredHeight = vPrevHeight;
        }
        else { // Cant derive width from itemviewPreviewSplitter or root ,as their width values are incorrect at this point in time
            previewHolder.Layout.preferredHeight = vPrevHeight = 300;
        }

        if (itemviewPreviewSplitter.orientation == Qt.Horizontal) {
            previewHolder.width = vPrevWidth
        }
        else {
            previewHolder.height = vPrevHeight
        }

        /* This ugly hack is the only way I could get the firstSplitViewLayout to resize correctly following a useContentsFrame toggle */
        useFolderFrame = !useFolderFrame
        useFolderFrame = !useFolderFrame
    }

    WGAssetFilterProxy {
        id: assetsModel
        objectName: "AssetsModel"
        sourceModel: typeof model == "undefined" ? null : model
        extension: nameFilter
        showHidden: root.showHidden
    }

    WGSortFilterProxy {
        id: folderModel
        objectName: "FolderModel"
        sourceModel: assetsModel
        filterObject : FolderFilterObject {
            filterName: "assetBrowserFolderFilter"
            filterRole: FolderFilterRole.Directory
        }
    }

    WGSortFilterProxy {
        id: filteredModel
        objectName: "FilteredModel"
        sourceModel: assetsModel

        filterObject: SearchFilterObject {
            filterName: "filteredModel"
            filterRole: searchFilterRole
            filterExpression: getFilterExpression(folderFilterString)
            parentFilter: folderFilterObject

            function filterAcceptsParentPath(path) {
                if(folderTree != null) {
                    var filteredFolderModelIndex = folderTree.getIndex(path);
                    var filteredModelIndex = filteredFolderModel.mapToSource(filteredFolderModelIndex);
                    var assetsModelIndex = filteredModel.mapToSource(filteredModelIndex);
                    var parent = assetsModelIndex.parent;
                    var row = assetsModelIndex.row;
                    return filteredModel.filterAcceptsRow(row, parent);
                }
                return false;
            }

            onFilterExpressionChanged: {
                if( activeFilters.dynamicSearchInProgress )
                {
                    filteredModel.invalidateFilter();
                    var rootIndex = folderTree.view.extendedModel.index(defaultSelectedFolderIndex, 0);
                    folderTree.expandRecursive(rootIndex, true );
                    return;
                }

                //save previous selection info
                var selectedFolderItemPath = currentFolder;
                var selectedContentItemPath = currentPath;

                //reset selection
                resetViews();

                // invalidate filter
                filteredModel.invalidateFilter();

                //restore selection if possible
                var selectionRestored = false;
                internal.shouldTrackFolderHistory = false;
                if(selectedContentItemPath != "") {
                    //restore contentView selection
                    var parentPath = WGPath.getParentPath(selectedContentItemPath);
                    if(filterAcceptsParentPath(parentPath)) {
                        selectionRestored = selectPath(selectedContentItemPath);
                    }
                } else if(selectedFolderItemPath != "") {
                    //restore folderTree selection
                    if(filterAcceptsParentPath(selectedFolderItemPath)) {
                        selectionRestored = selectFolder(selectedFolderItemPath);
                    }
                }
                internal.shouldTrackFolderHistory = true;

                if (!selectionRestored) {
                    // select first root folder
                    selectDefaultFolder();
                }
            }

            function getFilterExpression(searchFilter) {
                var tokens = searchFilter.split(activeFilters.splitterChar);
                var filterExpression = "";
                for (var i = 0; i < tokens.length; ++i) {
                    if (filterExpression.length > 0) {
                        filterExpression += "|";
                    }
                    if (tokens[i].length > 0) {
                        filterExpression += "(" + tokens[i] + ")";
                    }
                }
                return filterExpression;
            }
        }
    }

    WGSortFilterProxy {
        id: filteredFolderModel
        objectName: "FilteredFolderModel"
        sourceModel: filteredModel
        filterObject: folderFilterObject
    }

    WGSubProxy {
        id: contentsModel
        objectName: "ContentsModel"
    }

    WGFlattenProxy {
        id: flattenedContentsModel
        sourceModel: contentsModel
    }

    WGSortFilterProxy {
        id: searchModel
        sourceModel: flattenedContentsModel
        filterObject: InvertFilterObject {
            subFilter: folderFilterObject
        }
    }

    WGFastFilterProxy {
        id: searchResultsModel
        sourceModel: searchModel
        filterText: searchEdit.searchText
    }

    function resetViews() {
        if(contentView != null) {
            contentView.selectIndex(-1);
            contentView.contextMenu.contextObject = null;
        }

        if(folderTree != null) {
            folderTree.selectIndex(null);
            folderTree.contextMenu.contextObject = null;
        }

        currentFolder = "";
        currentPath = "";
    }

    function saveSelection() {
        resetTimer.previousFolder = currentFolder;
        resetTimer.previousPath = currentPath;
    }

    function restoreSelection() {
        internal.shouldTrackFolderHistory = false;
        if (resetTimer.previousPath == "" || !selectPath(resetTimer.previousPath)) {
            selectFolder(resetTimer.previousFolder);
        }
        internal.shouldTrackFolderHistory = true;
    }

    Timer {
        id: resetTimer
        interval: 0
        repeat: false

        property string previousFolder: ""
        property string previousPath: ""
        property bool entered: false

        onTriggered: {
            entered = false;
            restoreSelection();
        }
    }

    Connections {
        target: assetsModel

        onModelAboutToBeReset: {
            saveSelection();
            resetViews();
        }

        onModelReset: {
            resetTimer.start();
        }

        onRowsAboutToBeRemoved: {
            if(!resetTimer.entered) { // Use the timer to prevent entering multiple times during an update
                resetTimer.entered = true;
                saveSelection();
                resetViews();
            }
        }

        onRowsRemoved: {
            resetTimer.start();
        }

        onRowsAboutToBeInserted: {
            if(!resetTimer.entered) { // Use the timer to prevent enterng multiple times during an update
                resetTimer.entered = true;
                saveSelection();
                resetViews();
            }
        }

        onRowsInserted: {
            resetTimer.start();
        }
    }

    Connections {
        target: breadcrumbControl
        onBreadcrumbSelected: {
            folderTree.selectIndex(modelIndex);
        }
    }

    WGContextMenu {
        id: folderContextMenu
        path: folderContextMenuPath
    }

    WGContextMenu {
        id: assetContextMenu
        path: assetContextMenuPath
    }

    //--------------------------------------
    // View Parent Layout
    //--------------------------------------

    MouseArea
    {
        id: navigateBackForwardMouseArea
        anchors.fill: parent
        acceptedButtons: Qt.XButton1 | Qt.XButton2
        onClicked: {
            internal.onNavigate(mouse.button == Qt.XButton2)
        }
    }

    ColumnLayout {
        // Initial column layout with button/path bar at the top and then
        // the split two column panel underneath it.

        id: mainColumn
        objectName: "mainColumn"

        anchors.fill: parent
        anchors.margins: defaultSpacing.standardMargin

        WGExpandingRowLayout {
            // Contains both Row and column layout that internal controls are reparented to within checkAssetBrowserWidth
            id: resizeContainer
            Layout.fillWidth: true

            visible: showTopBar

            z: 1

            property bool singleLineLayout: true

            WGExpandingRowLayout {
                //contains all assetBrowserInfo in a single line
                id: assetBrowserInfoFirstLine
                Layout.fillWidth: true
                Layout.preferredHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize

                WGButtonBar {
                    evenBoxes: false
                    visible: showBackButton || showForwardButton || showRefreshButton || showHistoryButton

                    buttonList: [
                        // Breadcrumbs and back/forward
                        WGToolButton {
                            id: btnAssetBrowserBack
                            objectName: "backButton"
                            iconSource: "icons/back_16x16.png"
                            tooltip: "Back"
                            enabled: (internal.__currentFolderHistoryIndex > 0)
                            visible: showBackButton
                            onClicked: {
                                internal.onNavigate( false );
                            }
                        },
                        WGToolButton {
                            id: btnAssetBrowserForward
                            objectName: "forwardButton"
                            iconSource: "icons/fwd_16x16.png"
                            tooltip: "Forward"
                            enabled: (internal.__currentFolderHistoryIndex < folderHistory.count - 1)
                            visible: showForwardButton

                            onClicked: {
                                internal.onNavigate( true );
                            }
                        },
                        WGToolButton {
                            id: btnAssetBrowserRefresh
                            objectName: "refreshButton"
                            iconSource: "icons/loop_16x16.png"
                            tooltip: "Refresh"
                            width: 24
                            visible: showRefreshButton

                            onClicked: {
                                root.model.revert();
                                activeFilters.updateDefaultFiltersListing();
                            }
                        },
                        WGToolButton {
                            id: btnAssetBrowserHistory
                            objectName: "historyButton"
                            iconSource: "icons/arrow_down_small_16x16.png"
                            tooltip: "History"
                            width: 16
                            visible: showHistoryButton

                            showMenuIndicator: false

                            menu: WGMenu {
                                id: historyMenu

                                Instantiator {
                                    model: folderHistory
                                    delegate: MenuItem {
                                        text: path
                                        onTriggered: {
                                            internal.historyMenuItemClicked(index);
                                        }
                                    }
                                    onObjectAdded: historyMenu.insertItem(index, object)
                                    onObjectRemoved: historyMenu.removeItem(object)
                                }
                            }
                        }
                    ]
                }

                WGBreadcrumbs {
                    id: breadcrumbControl
                    model: filteredFolderModel
                    fontSize: breadCrumbsFontSize
                    visible: showBreadcrumbs
                }

                WGExpandingRowLayout {
                    id: assetBrowserPreferencesContainer
                    Layout.fillWidth: false

                    WGPushButton {
                        id: btnAssetBrowserHideOverlays
                        objectName: "btnAssetBrowserHideOverlays"
                        iconSource: showOverlayIcons ? "icons/overlays_on_16x16.png" : "icons/overlays_off_16x16.png"
                        tooltip: "Show Overlay Icons"

                        onClicked: {
                            showOverlayIcons = !showOverlayIcons
                        }
                        visible: showOverlayIconsButton
                    }

                    WGPushButton {
                        id: btnAssetBrowserShowHidden
                        objectName: "btnAssetBrowserShowHidden"
                        iconSource: showHidden ? "icons/hidden_files_on_16x16.png" : "icons/hidden_files_off_16x16.png"
                        tooltip: "Show Hidden"

                        onClicked: {
                            showHidden = !showHidden
                        }
                        visible: showHiddenButton
                    }

                    WGPushButton {
                        id: btnAssetBrowserHideThumbnails
                        objectName: "btnAssetBrowserHideThumbnails"
                        iconSource: showThumbnails ? "icons/thumbnail_16x16.png" : "icons/thumbnail_off_16x16.png"
                        tooltip: "Show Thumbnails"
                        enabled: showIcons

                        onClicked: {
                            showThumbnails = !showThumbnails
                        }
                        visible: showThumbnailsButton
                    }

                    WGPushButton {
                        id: btnAssetBrowserHideLabels
                        objectName: "btnAssetBrowserHideLabels"
                        iconSource: showLabels ? "icons/tag_on_16x16.png" : "icons/tag_off_16x16.png"
                        enabled: showIcons
                        tooltip: "Show Labels"

                        onClicked: {
                            showLabels = !showLabels
                        }
                        visible: showLabelButton
                    }

                    WGIconSizeMenu {
                        id: displayButton
                        Layout.preferredWidth: 100
                        assetBrowser: root
                        visible: showIconsButton
                    }

                    // Asset Browser View Options
                    WGPushButton {
                        id: btnAssetBrowserOrientation
                        objectName: "btnAssetBrowserOrientation"
                        iconSource: "icons/rows_16x16.png"
                        tooltip: "Horizontal/Vertical/Auto Toggle"
                        visible: haveFoldersAndContent && showOrientationButton

                        states: [
                            State {
                                name: "VERTICAL"
                                PropertyChanges { target: btnAssetBrowserOrientation; iconSource: "icons/rows_16x16.png" }
                            },
                            State {
                                name: "HORIZONTAL"
                                PropertyChanges { target: btnAssetBrowserOrientation; iconSource: "icons/columns_16x16.png" }
                            },
                            State {
                                name: "AUTO"
                                PropertyChanges { target: btnAssetBrowserOrientation; iconSource: "icons/align_a_16x16.png" }
                            }
                        ]

                        onClicked: {
                            if (assetViewOrientation == horizontalLayout) {
                                assetViewOrientation = verticalLayout
                                autoOrientation = false
                            }
                            else if (assetViewOrientation == verticalLayout) {
                                assetViewOrientation = autoLayout
                                autoOrientation = true
                                checkWidthToggleOrientation()
                            }
                            else if (assetViewOrientation == autoLayout) {
                                assetViewOrientation = horizontalLayout
                                autoOrientation = false
                            }
                            else { // catch for preferences in bad state
                                assetViewOrientation = defaultLayoutState
                                autoOrientation = false
                            }
                        }
                    }

                    WGPushButton {
                        id: btnAssetBrowserHideFolders
                        objectName: "btnAssetBrowserHideFolders"
                        iconSource: useFolderFrame ? "icons/folder_tree_16x16.png" : "icons/folder_tree_off_16x16.png"
                        tooltip: "Hide Folder List"
                        visible: haveFoldersAndContent && showHideFoldersButton

                        onClicked: {
                            if (!useContentsFrame && useFolderFrame)
                            {
                                useContentsFrame = true
                            }
                            useFolderFrame = !useFolderFrame
                        }
                    }

                    WGPushButton {
                        id: btnAssetBrowserHideContents
                        objectName: "btnAssetBrowserHideContents"
                        iconSource: useContentsFrame ? "icons/assets_on_16x16.png" : "icons/assets_off_16x16.png"
                        tooltip: "Hide Contents"
                        visible: haveFoldersAndContent && showHideContentsButton

                        onClicked: {
                            if (!useFolderFrame && useContentsFrame)
                            {
                                useFolderFrame = true
                            }
                            useContentsFrame = !useContentsFrame
                        }
                    }

                    WGPushButton {
                        id: btnAssetBrowserShowPreview
                        objectName: "btnAssetBrowserShowPreview"
                        iconSource: showPreview ? "icons/preview_16x16.png" : "icons/preview_off_16x16.png"
                        tooltip: "Show Preview"
                        checkable: true
                        checked: showPreview

                        onClicked: {
                            if(checked){
                                showPreview = true
                                callCheckPreviewWidthToggleOrientation()
                                checkPreviewValidWidthAndHeight()
                            } else {
                                showPreview = false
                            }
                        }

                        visible: showPreviewButton
                    }
                }
            }
        }

        // SplitView that breaks the panel up into two columns with draggable
        // handle.

        SplitView {
            id: assetSplitter
            objectName: "assetSplitter"
            Layout.fillHeight: true
            Layout.fillWidth: true
            orientation: Qt.Horizontal

            // Check that the firstSplitViewLayout width and height does not exceed that of the assetSplitter
            onWidthChanged: {
                checkValidWidthAndHeight()
            }

            onHeightChanged: {
                checkValidWidthAndHeight()
            }

            // Remember what size the user had it set at last before changing orientation
            onResizingChanged: {
                if(!resizing) { // execute once after resizing
                    if (assetSplitter.orientation == Qt.Horizontal) {
                        firstSplitViewLayout.Layout.preferredWidth = firstSplitViewLayout.width
                    }
                    else {
                        firstSplitViewLayout.Layout.preferredHeight = firstSplitViewLayout.height
                    }
                }
            }

            Component.onDestruction: {
                //TODO: directly use Preference when supporting dynamically add property for GeneircObject
                if (typeof viewId != "undefined" && viewId != null) {
                    addPreference(viewId, "assetViewOrientation", assetViewOrientation );
                    addPreference(viewId, "useContentsFrame", useContentsFrame? "true" : "false");
                }
            }

            states: [
                State {
                    name: "VERTICAL"
                    PropertyChanges { target: assetSplitter; orientation: Qt.Vertical }
                    PropertyChanges { target: firstSplitViewLayout; height: firstSplitViewLayout.Layout.preferredHeight }
                    PropertyChanges { target: firstSplitViewLayout; width: assetSplitter.width }
                },
                State {
                    name: "HORIZONTAL"
                    PropertyChanges { target: assetSplitter; orientation: Qt.Horizontal }
                    PropertyChanges { target: firstSplitViewLayout; height: assetSplitter.height }
                    PropertyChanges { target: firstSplitViewLayout; width: firstSplitViewLayout.Layout.preferredWidth }
                }
            ]

            // TODO Maybe should be a separate WG Component
            handleDelegate: Item {

                // A vertical line between two horizontally aligned frames
                property bool vertical: assetSplitter.orientation == Qt.Horizontal

                width: vertical ? defaultSpacing.separatorWidth + defaultSpacing.doubleBorderSize : assetSplitter.width
                height: vertical ? assetSplitter.height : defaultSpacing.separatorWidth + defaultSpacing.doubleBorderSize

                WGSeparator {
                    vertical: parent.vertical

                    anchors.centerIn: parent

                    width: vertical ? defaultSpacing.separatorWidth : parent.width
                    height: vertical ? parent.height : defaultSpacing.separatorWidth
                }
            }

            WGColumnLayout {
                id: firstSplitViewLayout
                visible: internal.useFolderFrame
                Layout.fillWidth: ((internal.useFolderFrame || useFolderFrame) && !internal.useContentsFrame) || assetSplitter.orientation == Qt.Vertical
                Layout.fillHeight: (internal.useFolderFrame && !internal.useContentsFrame) || assetSplitter.orientation == Qt.Horizontal

                Layout.minimumWidth: internal.useFolderFrame ? 50 : 0
                Layout.minimumHeight: internal.useFolderFrame ? 50 : 0

                clip: true

                WGActiveFilters {
                    id: activeFilters
                    objectName: "activeFilters"
                    dynamic: false // set false by default, to enable dynamic search in tree view, set it to true
                    Layout.fillWidth: true
                    Layout.minimumWidth: parent.Layout.minimumWidth
                    Layout.minimumHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize

                    onBeforeAddFilter: {
                        // Escape regular expression special characters
                        filter.value = RegExp.escape(filter.value)
                    }

                    autoCompleteModel: WGFlattenProxy {
                        sourceModel: folderModel
                    }
                    autoCompleteRole: "fullPath"

                }

                WGScrollView {
                    id: folderView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.minimumWidth: parent.Layout.minimumWidth
                    // To get this minimumHeight to mactch firstSplitViewLayout a magic number was required!
                    Layout.minimumHeight: parent.Layout.minimumHeight -10

                    Component.onDestruction: {
                        //TODO: directly use Preference when supporting dynamically add property for GenericObject
                        if(typeof viewId != "undefined" && viewId != null) {
                            addPreference(viewId, "leftFrameVisible", useFolderFrame ? "true" : "false" );
                            addPreference(viewId, "showHiddenFolders", showHidden );
                            if (assetSplitter.orientation == Qt.Horizontal)
                            {
                                if (internal.useContentsFrame) {
                                    addPreference(viewId, "leftFrameWidth", firstSplitViewLayout.width );
                                }
                                else { // Retain the preferred width for next session
                                    addPreference(viewId, "leftFrameWidth", firstSplitViewLayout.Layout.preferredWidth );
                                }
                                //The preferred width now remembers what it was last time the user changed the orientation.. intra-session
                                addPreference(viewId, "leftFrameHeight", firstSplitViewLayout.Layout.preferredHeight );

                            }
                            else
                            {
                                if (internal.useContentsFrame) {
                                    addPreference(viewId, "leftFrameHeight", firstSplitViewLayout.height );
                                }
                                else { // Retain the preferred width for next session
                                    addPreference(viewId, "leftFrameHeight", firstSplitViewLayout.Layout.preferredHeight );
                                }
                                addPreference(viewId, "leftFrameWidth", firstSplitViewLayout.Layout.preferredWidth );
                            }

                        }
                    }

                    WGLoader {
                        id: folderLoader
                        focus: true

                        sourceComponent: assetFolderView

                        onStatusChanged: {
                            if (status == Loader.Ready)
                            {
                                item.model = Qt.binding(function() { return filteredFolderModel })
                                item.expandToDepth = 1
                                folderView.contentItem = item
                                item.assetBrowser = root
                                item.contextMenu = folderContextMenu
                                if(currentFolder == "")
                                {
                                    // select first root folder
                                    selectDefaultFolder();
                                }
                            }
                        }

                    }
                }
            }

            WGColumnLayout {
                id: secondSplitViewLayout
                Layout.fillWidth: internal.useContentsFrame
                Layout.fillHeight: internal.useContentsFrame
                Layout.minimumWidth: internal.useContentsFrame ? iconSize + defaultSpacing.doubleMargin : 0
                visible: internal.useContentsFrame
                enabled: internal.useContentsFrame

                WGExpandingRowLayout {

                    WGTextBoxAutoComplete {
                        id: searchEdit
                        objectName: "searchEdit"
                        Layout.fillWidth: true
                        Layout.minimumWidth: 50
                        Layout.minimumHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize
                        placeholderText: "Search"
                        showSuggestions: false
                        multiText: true

                        style: WGTextBoxStyle {
                            textColor: searchEdit.searchTextColor
                        }

                        suggestionModel: searchResultsModel

                        //TODO: enable once p4 integration is not such a performance drain
                        property var dynamicSearch: true
                        property var searchText: dynamicSearch ? selectionStart > 0 ? text.substring(0, selectionStart) : text : ""

                        property color searchTextColor: searchText == text && searchText != "" ? palette.highlightColor : palette.textColor

                        onTextChanged: {
                            if(dynamicSearch)
                            {
                                searchText = selectionStart > 0 ? text.substring(0, selectionStart) : text;
                                return;
                            }
                            if (text == "")
                            {
                                searchText = ""
                            }
                        }

                        onEditAccepted: {
                            searchText = text;
                        }

                        WGToolButton {
                            id: searchClearButton
                            anchors.right: parent.right
                            anchors.rightMargin: defaultSpacing.standardBorderSize
                            anchors.verticalCenter: parent.verticalCenter
                            iconSource: "icons/close_sml_16x16.png"
                            visible: searchEdit.text != ""
                            onClicked: {
                                searchEdit.text = ""
                            }
                        }
                    }

                    WGImage {
                        source: nameFiltersDropDown.currentIndex > 0 ? "icons/filter_enabled_16x16.png" : "icons/filter_16x16.png"
                        visible: nameFilters != null
                    }

                    WGLabel {
                        text: nameFiltersLabel + ":"
                        visible: nameFilters != null
                        Layout.alignment: Qt.AlignVCenter
                    }

                    WGDropDownBox {
                        id: nameFiltersDropDown
                        objectName: "fileExtensionFilter"
                        Layout.preferredWidth: 130
                        Layout.alignment: Qt.AlignTop
                        visible: nameFilters != null

                        model: nameFilters
                        textRole: Array.isArray(model) ? "" : "value"

                        __textColor: currentIndex > 0 ? palette.highlightColor : palette.neutralTextColor

                        Component.onCompleted: {
                            if(selectedNameFilter == "")
                            {
                                currentIndex = 0
                            }
                            else
                            {
                                currentIndex = find(selectedNameFilter);
                            }
                            selectedNameFilter = currentText;
                        }

                        onActivated: {
                            selectedNameFilter = currentText;
                        }
                    }
                }

                SplitView {
                    id: itemviewPreviewSplitter
                    objectName: "itemviewPreviewSplitter"
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    orientation: Qt.Horizontal

                    function checkPreviewWidthToggleOrientation() {
                        if (previewAutoOrientation)
                        {
                            if (width < height * 0.8 && itemviewPreviewSplitter.state == "HORIZONTAL")
                            {
                                itemviewPreviewSplitter.state = "VERTICAL"
                            }
                            else if (width > height * 1.2 && itemviewPreviewSplitter.state == "VERTICAL")
                            {
                                itemviewPreviewSplitter.state = "HORIZONTAL"
                            }
                        }
                    }

                    Component.onDestruction: {
                        //TODO: directly use Preference when supporting dynamically add property for GenericObject
                        if(typeof viewId != "undefined" && viewId != null) {
                            addPreference(viewId, "previewViewOrientation", previewViewOrientation );
                            addPreference(viewId, "showPreview", showPreview );
                            addPreference(viewId, "assetIconSize", iconSize );
                            if (itemviewPreviewSplitter.orientation == Qt.Horizontal)
                            {
                                addPreference(viewId, "assetPreviewWidth", previewHolder.width );
                                addPreference(viewId, "assetPreviewHeight", previewHolder.Layout.preferredHeight );

                            } else
                            {
                                addPreference(viewId, "assetPreviewHeight", previewHolder.height );
                                addPreference(viewId, "assetPreviewWidth", previewHolder.Layout.preferredWidth );
                            }
                        }
                    }

                    onResizingChanged: {
                        if (!resizing) // execute after resizing
                        {
                            if (itemviewPreviewSplitter.orientation == Qt.Horizontal) {
                                previewHolder.Layout.preferredWidth = previewHolder.width
                            }
                            else {
                                previewHolder.Layout.preferredHeight = previewHolder.height
                            }
                        }
                    }


                    onWidthChanged: {
                        checkPreviewState()
                        checkPreviewValidWidthAndHeight()
                        checkPreviewWidthToggleOrientation()
                    }

                    onHeightChanged: {
                        checkPreviewState()
                        checkPreviewValidWidthAndHeight()
                        checkPreviewWidthToggleOrientation()
                    }


                    states: [
                        State {
                            name: "VERTICAL"
                            PropertyChanges { target: itemviewPreviewSplitter; orientation: Qt.Vertical }
                            PropertyChanges { target: previewHolder; height: previewHolder.Layout.preferredHeight }
                            PropertyChanges { target: previewHolder; width: itemviewPreviewSplitter.width }

                        },
                        State {
                            name: "HORIZONTAL"
                            PropertyChanges { target: itemviewPreviewSplitter; orientation: Qt.Horizontal }
                            PropertyChanges { target: previewHolder; width: previewHolder.Layout.preferredWidth }
                            PropertyChanges { target: previewHolder; height: itemviewPreviewSplitter.height }
                        }
                    ]

                    // TODO Maybe should be a separate WG Component
                    handleDelegate: Item {

                        // yes this is reversed. Blame the default SplitView for being stupid.
                        property bool vertical: itemviewPreviewSplitter.orientation == Qt.Horizontal

                        width: vertical ? defaultSpacing.separatorWidth + defaultSpacing.doubleBorderSize : itemviewPreviewSplitter.width
                        height: vertical ? itemviewPreviewSplitter.height : defaultSpacing.separatorWidth + defaultSpacing.doubleBorderSize

                        WGSeparator {
                            vertical: parent.vertical

                            anchors.centerIn: parent

                            width: vertical ? defaultSpacing.separatorWidth : parent.width
                            height: vertical ? parent.height : defaultSpacing.separatorWidth
                        }
                    }


                    Item {
                        id: assetFrame
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        Layout.minimumHeight: internal.useContentsFrame ? iconSize + iconLabelSize : 0
                        Layout.minimumWidth: internal.useContentsFrame ? iconSize + defaultSpacing.doubleMargin : 0

                        WGScrollView {
                            id: assetScrollView
                            anchors.fill: parent
                            contentItem: assetView.item

                            WGLoader {
                                id: assetView
                                focus: true
                                sourceComponent: showIconsButton && showIcons ? assetIconView : assetListView

                                onStatusChanged: {
                                    if (status == Loader.Ready)
                                    {
                                        item.assetBrowser = root
                                        item.contextMenu = assetContextMenu
                                        if (sourceComponent == assetIconView) {
                                            item.internalSelectionModel = Qt.binding(function() { return iconViewSelectionModel })
                                        }
                                        else if (sourceComponent == assetListView) {
                                            item.internalSelectionModel = Qt.binding(function() { return listViewSelectionModel })
                                        }
                                        item.model = Qt.binding(function() { return contentViewModel })
                                        assetScrollView.contentItem = item
                                    }
                                }
                            }
                        }
                    }
                    Item {
                        id: previewHolder
                        Layout.minimumHeight: showPreview ? 50 : 0
                        Layout.minimumWidth: showPreview ? 50 : 0
                        Layout.fillHeight: itemviewPreviewSplitter.orientation == Qt.Horizontal
                        Layout.fillWidth: itemviewPreviewSplitter.orientation == Qt.Vertical

                        Layout.maximumWidth: {
                            var maxW = itemviewPreviewSplitter.width - 50
                            if (itemviewPreviewSplitter.orientation == Qt.Vertical)
                            {
                                 maxW = -1
                            }
                            return maxW
                        }
                        Layout.maximumHeight: {
                            var maxH = itemviewPreviewSplitter.height - 50
                            if (itemviewPreviewSplitter.orientation == Qt.Horizontal)
                            {
                                 maxH = -1
                            }
                            return maxH
                        }

                        visible: showPreview

                        WGLoader {
                            id: previewLoader
                            anchors.fill: parent

                            sourceComponent: previewPanel

                            onStatusChanged: {
                                item.assetBrowser = root
                            }
                        }
                    }
                } // SplitView
            }
        } //SplitView
    }

    Connections{
        target: contentView
        onItemSelected: {
            currentPath = getFullPath(item);
        }
    }

    Connections{
        target: folderTree
        onItemSelected: {
            var previousFolder = currentFolder;
            currentFolder = getFullPath(item);
            if (currentFolder == previousFolder) {
                // Temporary check to avoid added the same item into the folder history
                // This can go away when we stop artificially sending selection changes on right clicks
                return;
            }

            var extendedIndex = folderTree.view.extendedModel.itemToIndex(item);
            var proxyIndex = folderTree.view.sourceIndex(extendedIndex);
            var filteredFolderModelIndex = folderTree.view.proxyModel.mapToSource(proxyIndex);
            var filteredModelIndex = filteredFolderModel.mapToSource(filteredFolderModelIndex);

            breadcrumbControl.modelIndex = filteredFolderModelIndex;
            contentsModel.sourceParent = filteredModelIndex;

            if (internal.shouldTrackFolderHistory && currentFolder != "") {
                // Prune history as needed based on maximum length allowed
                if (folderHistory.count >= maxHistoryItems) {
                    folderHistory.remove(0);
                }

                // Track the folder selection indices history
                folderHistory.append({"path" : currentFolder});
                internal.__currentFolderHistoryIndex = folderHistory.count - 1;
            }
        }
    }

    /** Allows selecting an item from the browser programatically */
    property var programSelectionPath: typeof(programSelectedItemPath) == "undefined" ? "" : programSelectedItemPath
    onProgramSelectionPathChanged: {
        if(programSelectionPath != "") {
            if(!selectPath(programSelectionPath)) {
                WGLogger.logError("Could not select path " + programSelectionPath);
            }
        }
    }

    property var programSelectionIds: typeof(programSelectedIds) == "undefined" ? null : programSelectedIds
    onProgramSelectionIdsChanged: {
        if (contentView == null) {
            return;
        }
        var count = programSelectionIds != null ? programSelectionIds.count() : 0;
        if (count == 0)
        {
            contentView.clearSelection();
        }
        else if (count > 0)
        {
            var identifiers = programSelectionIds.values();

            // call selectPath on the first item to expand and bring it in to focus
            var index = root.model.findIndex(identifiers[0]);
            var item = root.model.indexToItem(index);
            var fullPath = getFullPath(item);
            if(!selectPath(fullPath) && fullPath != "") {
                WGLogger.logError("Could not select path " + fullPath);
            }

            var indexes = root.model.findIndexes(identifiers);
            var newSelection = assetView.item.view.toItemSelection(indexes)
            contentsSelectionModel.select(newSelection, ItemSelectionModel.Clear | ItemSelectionModel.Select);

            var result = contentView.selectionModel.updateIndexes();
            if (!result.selectionChanged)
            {
                // Force a selection change to handle selecting items outside of the current model,
                // Relevant for streaming editor, i.e. selecting from another folder/world
                contentView.selectionChanged();
            }
        }
    }
}

