import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1
import WGControls 1.0

/*!
 \brief A control used for display, browsing and interaction of assets on disc.

----------------------------------------------------------------------------------------------
 Preliminary Layout Designed but not Finalised! Icons and menus currently placeholders only.

 It is operating off data models for file folders and the listview. The layout is based upon:
		Search for Asset Browser Qt Prototype on the Wargaming Confluence
----------------------------------------------------------------------------------------------

Example:
\code{.js}
WGAssetBrowser {
    id: testAssetBrowserControl
    viewModel: view
}
\endcode
*/

//ToDo: Better naming of ObjectName for the thumbnails and repeated elements within assetbrowser using itemData

Rectangle {
    id: rootFrame
    objectName: "WGAssetBrowser"
    color: palette.mainWindowColor

    //Public properties
    /*! This property holds the viewModel containing the assets to be displayed*/
    property var viewModel;

    /*! This property determines the default size of the icons in the listview
        The default value depends on the implementation of the viewModel
    */
    property int iconSize: viewModel.data.iconSize

    /*! This property determines the size of the label of each icon */
    property int iconLabelSize: iconSize > 32 ? 9 : 7

    /*! This property determines the line height for each icon label */
    property int iconLabelLineHeight: iconSize > 32 ? 16 : 10

    /*!  This property indicates if the asset browser is showing a grid (true) or a list view (false) */
    readonly property bool showIcons: iconSize >= 32

    /*! This property determines the maximum number of history items tracked during asset tree navigation
        The default value is \c 10 */
    property int maxHistoryItems: 10

    /*! Property determines if history items should be stored or not. For example, if you are
        selecting a top-level breadcrumb, you don't want to re-track that as a history item. You also wouldn't
        want to re-track items as you move forward or backward through the history. You do however want to track
        new selections or child breadcrumb selections. */
    property bool shouldTrackFolderHistory: true

    //--------------------------------------
    // Custom Content Filters
    //--------------------------------------
    // Note: This will be replaced with a more robust filtering system in the near future.

    /*! This property exposes the active filters control to any outside resources that may need it. */
    property var activeFilters_: activeFilters

    /*! \internal */
    property QtObject contentDisplayType: ListModel {
        // Start with 'List View'
        property int currentIndex_: 1
        ListElement {
            name_: "displayType1_"
            text: "Icons"
        }
        ListElement {
            name_: "displayType2_"
            text: "List View"
        }
    }

    // Local Variables to Keep track of folder TreeModel selection indices history
    /*! \internal */
    property var __folderHistoryIndices: new Array()

    /*! \internal */
    property int __currentFolderHistoryIndex: 0

    /*! \internal */
    property int __maxFolderHistoryIndices: 0

    onWidthChanged: checkAssetBrowserWidth()

    Component.onCompleted: {
        var value = preference.assetViewOrientation;
        if (typeof value != "undefined")
        {
            assetSplitter.state = value;
            btnAssetBrowserOrientation.checked = assetSplitter.orientation ==  Qt.Vertical;
        }

        var vVisible = preference.leftFrameVisible;
        var vWidth = preference.leftFrameWidth;
        var vHeight = preference.leftFrameHeight;
        if ((typeof vWidth != "undefined") && (typeof vHeight != "undefined")&& (typeof vVisible != "undefined"))
        {
            leftFrame.visible = (vVisible == "true");
            btnAssetBrowserHideFolders.checked = !(vVisible == "true");
            leftFrame.width = vWidth;
            leftFrame.height = vHeight;
        }
    }

    //--------------------------------------
    // Functions
    //--------------------------------------

    /*! \internal */
    function checkAssetBrowserWidth() {
        // Change breadcrumbs and preferences to double line layout
        if (resizeContainer.singleLineLayout == true)
        {
            var changingLayout = (breadcrumbControl.breadcrumbRepeater_.count > 0 &&
                                  breadcrumbControl.breadcrumbRowLayout_.width > breadcrumbControl.width)
            if (changingLayout)
            {
                // reparent breadcrumb group to its own rowlayout
                assetBrowserInfoFirstLine.parent = assetBrowserDoubleLineColumn
                assetBrowserInfoSecondLine.parent = assetBrowserDoubleLineColumn

                //reparent preferences group to its own rowlayout
                assetBrowserPreferencesContainer.parent = assetBrowserInfoSecondLine

                //visibility change
                assetBrowserDoubleLineColumn.visible = true
                assetBrowserInfoSecondLine.visible = true

                resizeContainer.singleLineLayout = false
            }
        }
        else // Change breadcrumbs and preferences to single line layout
        {
            var changingLayoutagain = (breadcrumbControl.breadcrumbRepeater_.count > 0 &&
                                       breadcrumbControl.width - breadcrumbControl.breadcrumbRowLayout_.width >
                                       assetBrowserInfoSecondLine.childrenRect.width)
            if (changingLayoutagain)
            {
                // reparent everything to a single row
                assetBrowserInfoFirstLine.parent = resizeContainer
                assetBrowserInfoSecondLine.parent = resizeContainer
                assetBrowserDoubleLineColumn.parent = resizeContainer

                assetBrowserPreferencesContainer.parent = assetBrowserInfoFirstLine

                //visibility swap
                assetBrowserDoubleLineColumn.visible = false
                assetBrowserInfoSecondLine.visible = false

                resizeContainer.singleLineLayout = true
            }
        }
    }

    // Selects an asset from the folder contents view
    /*! \internal */
    function selectAsset( index ){
        rootFrame.viewModel.currentSelectedAssetIndex = index;

        // Prepare the menu for display by querying selected asset
        // data and determine what the menu should include or exclude
        // TODO: This functionality should be moved to the WGMenu
        //       after Qt controls v1.4 is added to NGT.
        rootFrame.viewModel.contextMenu.prepareMenu;
    }

    // Tells the page to use the current selected asset
    function onUseSelectedAsset() {
        rootFrame.viewModel.events.useSelectedAsset( listModelSelection.selectedItem );
    }

    // Tells the page to navigate the history forward or backward
    // depending on what button was clicked
    function onNavigate( isForward ) {
        // Don't navigate if we're actively filtering assets
        if (folderContentsModel.isFiltering) {
            return;
        }

        // Don't track the folder history while we use the navigate buttons the history
        rootFrame.shouldTrackFolderHistory = false;

        if (isForward) {
            if (folderHistoryIndices.length > __currentFolderHistoryIndex + 1) {
                __currentFolderHistoryIndex += 1;
                selector.selectedIndex = __folderHistoryIndices[__currentFolderHistoryIndex];
            }

        }
        else {
            if (__currentFolderHistoryIndex > -1) {
                __currentFolderHistoryIndex -= 1;
                selector.selectedIndex = __folderHistoryIndices[__currentFolderHistoryIndex];
            }
        }
    }

    // We need to update the asset manager view
    function onRefresh( ) {

        rootFrame.viewModel.refreshData;
    }

    // Handles a history menu item being clicked
    function historyMenuItemClicked( index ) {
        // Don't navigate if we're actively filtering assets
        if (folderContentsModel.isFiltering) {
            return;
        }

        // Make sure the index is valid
        if (folderHistoryIndices.length <= index) {
            return;
        }

        // Don't track the folder history while navigating said history
        rootFrame.shouldTrackFolderHistory = false;

        __currentFolderHistoryIndex = index;
        selector.selectedIndex = __folderHistoryIndices[__currentFolderHistoryIndex];
    }

    // Handles breadcrumb selection
    function handleBreadcrumbSelection( index, childIndex ) {
        // Do not navigate if we are filtering assets
        if (folderContentsModel.isFiltering) {
            return;
        }

        // Don't track the folder history while we navigate the history unless it's a submenu (treated as a new
        // selection)
        if (childIndex > -1) {
            rootFrame.shouldTrackFolderHistory = false;
        }

        // Get the IItem from the selected breadcrumb and convert it into a QModelIndex that
        // can be used for selection
        var item = rootFrame.viewModel.breadcrumbsModel.getItemAtIndex( index, childIndex );
        var qModelIndex = folderModel.convertItemToIndex( item );

        // Make the new selection
        selector.selectedIndex = qModelIndex;
    }

    ListModel { id: folderHistoryNames }

    WGListModel {
        id: customContentFiltersModel
        source: rootFrame.viewModel.data.customContentFilters
        ValueExtension {}
    }

    ListModel {
        id: customContentFiltersList

        Component.onCompleted: {
            var filterItr = iterator( rootFrame.viewModel.data.customContentFilters )
            while (filterItr.moveNext()) {
                customContentFiltersList.append({
                    text: filterItr.current
                });
            }
        }
    }

    WGDataChangeNotifier {
        id: customContentFilterIndexNotifier
        source: rootFrame.viewModel.data.customContentFilterIndexNotifier
        onDataChanged: {
            rootFrame.viewModel.refreshData;
        }
    }

    //--------------------------------------
    // Folder Tree Model
    //--------------------------------------
    WGFilteredTreeModel {
        id : folderModel
        objectName: "AssetBrowserTreeModel"
        source : rootFrame.viewModel.data.folders

        filter: WGTokenizedStringFilter {
            id: folderFilter
            filterText: activeFilters_.stringValue
            itemRole: "value"
            splitterChar: ","
        }

        ValueExtension {}
        ColumnExtension {}
        ComponentExtension {}
        TreeExtension {
            id: folderTreeExtension
            selectionExtension: selector
        }

        ThumbnailExtension {}
        SelectionExtension {
            id: selector
            onSelectionChanged: {
                if (!folderTreeExtension.blockSelection && !folderContentsModel.isFiltering)
                {
                    // Source change
                    folderModelSelectionHelper.select(getSelection());
                    if (rootFrame.shouldTrackFolderHistory)
                    {
                        // Prune history as needed based on maximum length allowed
                        if (folderHistoryIndices.length >= maxHistoryItems) {
                            __folderHistoryIndices.splice(0, 1);
                            folderHistoryNames.remove(0);
                        }

                        // Track the folder selection indices history
                        __folderHistoryIndices.push(selector.selectedIndex);
                        folderHistoryNames.append({"name" : rootFrame.viewModel.getSelectedTreeItemName()});
                        __currentFolderHistoryIndex = __folderHistoryIndices.length - 1;
                        __maxFolderHistoryIndices = __folderHistoryIndices.length - 1;
                    }

                    // Reset the flag to track the folder history
                    rootFrame.shouldTrackFolderHistory = true;
                }

                folderTreeExtension.blockSelection = false;
            }
        }
    }

    WGSelectionHelper {
        id: folderModelSelectionHelper
        source: rootFrame.viewModel.folderSelectionHandler
        onSourceChanged: {
            select(selector.getSelection());
        }
    }


    //--------------------------------------
    // List View Model for Folder Contents
    //--------------------------------------
    WGFilteredListModel {
        id : folderContentsModel

        source : rootFrame.viewModel.data.folderContents
        filter: WGTokenizedStringFilter {
            id: folderContentsFilter
            filterText: activeFilters_.stringValue
            itemRole: "value"
            splitterChar: ","
        }

        onFilteringBegin: {
            folderTreeExtension.blockSelection = true;
        }

        onFilteringEnd: {
            folderTreeExtension.blockSelection = false;
        }

        ValueExtension {}
        AssetItemExtension {}

        ColumnExtension {}
        ComponentExtension {}
        ThumbnailExtension {}
        SelectionExtension {
            id: listModelSelection
            multiSelect: true
            onSelectionChanged: {
                fileModelSelectionHelper.select(getSelection());

                // Prepare the context menu by passing the selected asset from the
                // list model and telling the menu to show, which will update
                // the actions data with the selected asset for processing.
                contextMenu.contextObject = listModelSelection.selectedItem;
            }

            onCurrentIndexChanged: {
                listModelSelection.selectedIndex = currentIndex;
            }
        }
    }

    WGSelectionHelper {
        id: fileModelSelectionHelper
        source: rootFrame.viewModel.folderContentSelectionHandler
        onSourceChanged: {
            select(listModelSelection.getSelection());
        }
    }

    //--------------------------------------
    // List Model for recent file history
    //--------------------------------------
    WGListModel {
        id: recentFileHistoryModel
        source: rootFrame.viewModel.recentFileHistory

        ValueExtension {}
        ColumnExtension {}
        ComponentExtension {}
        TreeExtension {}
        ThumbnailExtension {}
        SelectionExtension {
            id: recentFileHistorySelection
            multiSelect: false
        }
    }

    WGDataChangeNotifier {
        id: folderSelectionHistory
        source: rootFrame.viewModel.folderSelectionHistoryIndex

        // Update the current item index when we get this data change notify
        onDataChanged: {
            __currentFolderHistoryIndex = data;

            // Update the folder TreeModel selectedIndex
            selector.selectedIndex = __folderHistoryIndices[data];
        }
    }


    //--------------------------------------
    // Context Menu
    //--------------------------------------
    WGContextArea {
        id: fileContextMenu
        objectName: "fileContextMenu"
        onAboutToShow: {
            // Prepare the context menu by passing the selected asset from the
            // list model and telling the menu to show, which will update
            // the actions data with the selected asset for processing.
            contextMenu.contextObject = listModelSelection.selectedItem;
        }
        WGContextMenu {
            id: contextMenu
            path: "WGAssetBrowserAssetMenu"
        }
    }


    //--------------------------------------
    // View Parent Layout
    //--------------------------------------

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

            z: 1

            property bool singleLineLayout: true

            WGExpandingRowLayout {
                //contains all assetBrowserInfo in a single line
                id: assetBrowserInfoFirstLine
                Layout.fillWidth: true
                Layout.preferredHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize

                WGButtonBar {
                    evenBoxes: false

                    buttonList: [
                        // Breadcrumbs and back/forward
                        WGToolButton {
                            id: btnAssetBrowserBack
                            objectName: "backButton"
                            iconSource: "icons/back_16x16.png"
                            tooltip: "Back"
                            enabled: (__currentFolderHistoryIndex != 0)

                            onClicked: {
                                onNavigate( false );
                            }
                        },
                        WGToolButton {
                            id: btnAssetBrowserForward
                            objectName: "forwardButton"
                            iconSource: "icons/fwd_16x16.png"
                            tooltip: "Forward"
                            enabled: (__currentFolderHistoryIndex < __folderHistoryIndices.length - 1)


                            onClicked: {
                                onNavigate( true );
                            }
                        },
                        WGToolButton {
                            id: btnAssetBrowserRefresh
                            objectName: "refreshButton"
                            iconSource: "icons/loop_16x16.png"
                            tooltip: "Refresh"
                            width: 16


                            onClicked: {
                                onRefresh();
                            }
                        },
                        WGToolButton {
                            id: btnAssetBrowserHistory
                            objectName: "historyButton"
                            iconSource: "icons/arrow_down_small_16x16.png"
                            tooltip: "History"
                            width: 16

                            showMenuIndicator: false

                            menu: WGMenu {
                                id: historyMenu

                                Instantiator {
                                    model: folderHistoryNames
                                    delegate: MenuItem {
                                        text: name
                                        onTriggered: {
                                            historyMenuItemClicked(index);
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
                    dataModel: rootFrame.viewModel.breadcrumbsModel

                    onBreadcrumbClicked: {
                        handleBreadcrumbSelection( index, -1 );
                    }

                    onBreadcrumbChildClicked: {
                        handleBreadcrumbSelection( index, childIndex );
                    }

                    onBreadcrumbPathEntered: {
                        // Do not navigate if we are filtering assets
                        if (folderContentsModel.isFiltering) {
                            return;
                        }

                        rootFrame.shouldTrackFolderHistory = true;

                        // Get the IItem for the asset at the designated path
                        var itemAtPath = rootFrame.viewModel.data.findAssetWithPath(path.toString());
                        var qModelIndex = folderModel.convertItemToIndex( itemAtPath );

                        // Make the new selection
                        selector.selectedIndex = qModelIndex;
                    }
                }

                WGExpandingRowLayout {
                    id: assetBrowserPreferencesContainer
                    Layout.fillWidth: false

                    WGPushButton {
                        id: displayButton
                        objectName: "displayButton"
                        Layout.preferredWidth: 100
                        checkable: true

                        text: showIcons ? (iconSize + "px Icons") : "List View"

                        Timer {
                            id: fadeTimer
                            running: false
                            interval: 1000

                            onTriggered: {
                                displayButton.checked = false
                            }
                        }

                        onActiveFocusChanged: {
                            if(!activeFocus)
                            {
                                displayButton.checked = false
                            }
                        }

                        // IconSize control pulldown
                        Rectangle {
                            id: sizeMenu
                            anchors.left: displayButton.left
                            anchors.top: displayButton.bottom
                            visible: displayButton.checked
                            height: 120
                            width: 140


                            color: palette.mainWindowColor
                            border.width: defaultSpacing.standardBorderSize
                            border.color: palette.darkColor

                            MouseArea {
                                // prevents items being selected behind drop down
                                id: catchMouseArea
                                anchors.fill: parent
                                propagateComposedEvents: false

                                hoverEnabled: true

                                onEntered: {
                                    fadeTimer.stop()
                                }

                                onExited: {
                                        fadeTimer.restart()
                                }
                            }

                            WGSlider {
                                id: slider
                                objectName: "slider"
                                stepSize: 32
                                minimumValue: 0
                                maximumValue: 256
                                width: 16
                                height: menuItems.childrenRect.height
                                orientation: Qt.Vertical
                                anchors.top: sizeMenu.top
                                anchors.left: sizeMenu.left
                                anchors.margins: {left: 2; right: 2; top: 5; bottom: 5}
                                rotation: 180

                                WGSliderHandle {
                                    id: sliderHandle
                                    objectName: "sliderHandle"
                                    minimumValue: slider.minimumValue
                                    maximumValue: slider.maximumValue
                                    showBar: true

                                    onValueChanged: {
                                        rootFrame.iconSize = value
                                    }

                                    Binding {
                                        target: sliderHandle
                                        property: "value"
                                        value: rootFrame.iconSize
                                    }
                                }
                            }

                            // Iconsize Pulldown Buttons
                            Item {
                                id: menuItems
                                height: childrenRect.height
                                width: 110
                                anchors.top: sizeMenu.top
                                anchors.left: slider.right
                                anchors.margins: {left: 2; right: 2; top: 5; bottom: 5}

                                WGPushButton {
                                    id: listViewButton
                                    objectName: "listViewButton"
                                    anchors.top: menuItems.top
                                    anchors.left: menuItems.left
                                    width: menuItems.width
                                    text: "List View"

                                    onHoveredChanged: {
                                        if (hovered) {
                                            fadeTimer.stop()
                                        }
                                        else {
                                            fadeTimer.restart()
                                        }
                                    }

                                    onClicked: {
                                        iconSize = 0
                                    }
                                }
                                WGPushButton {
                                    id: smallIconsButton
                                    objectName: "smallIconsButton"
                                    anchors.top: listViewButton.bottom
                                    anchors.left: menuItems.left
                                    width: menuItems.width
                                    text: "Small Icons"

                                    onHoveredChanged: {
                                        if (hovered) {
                                            fadeTimer.stop()
                                        }
                                        else {
                                            fadeTimer.restart()
                                        }
                                    }

                                    onClicked: {
                                        iconSize = 48
                                    }
                                }
                                WGPushButton {
                                    id: mediumIconsButton
                                    objectName: "mediumIconsButton"
                                    anchors.top: smallIconsButton.bottom
                                    anchors.left: menuItems.left
                                    width: menuItems.width
                                    text: "Medium Icons"

                                    onHoveredChanged: {
                                        if (hovered) {
                                            fadeTimer.stop()
                                        }
                                        else {
                                            fadeTimer.restart()
                                        }
                                    }

                                    onClicked: {
                                        iconSize = 96
                                    }
                                }
                                WGPushButton {
                                    id: largeIconsButton
                                    objectName: "largeIconsButton"
                                    anchors.top: mediumIconsButton.bottom
                                    anchors.left: menuItems.left
                                    width: menuItems.width
                                    text: "Large Icons"

                                    onHoveredChanged: {
                                        if (hovered) {
                                            fadeTimer.stop()
                                        }
                                        else {
                                            fadeTimer.restart()
                                        }
                                    }

                                    onClicked: {
                                        iconSize = 128
                                    }
                                }
                                WGPushButton {
                                    id: extraLargeIconsButton
                                    objectName: "extraLargeIconsButton"
                                    anchors.top: largeIconsButton.bottom
                                    anchors.left: menuItems.left
                                    width: menuItems.width
                                    text: "Extra Large Icons"

                                    onHoveredChanged: {
                                        if (hovered) {
                                            fadeTimer.stop()
                                        }
                                        else {
                                            fadeTimer.restart()
                                        }
                                    }

                                    onClicked: {
                                        iconSize = 256
                                    }
                                }
                            }
                        }

                        MouseArea {
                            id: sliderCoverMouseArea
                            objectName: "sliderCoverMouseArea"

                            anchors.top: parent.top
                            anchors.left: sizeMenu.left
                            anchors.bottom: sizeMenu.bottom
                            width: sizeMenu.width - menuItems.width
                            propagateComposedEvents: true

                            hoverEnabled: displayButton.checked

                            acceptedButtons: Qt.NoButton

                            onEntered: {
                                fadeTimer.stop()
                            }
                            onExited: {
                                fadeTimer.restart()
                            }

                            onWheel: {
                                if (wheel.angleDelta.y > 0)
                                {
                                    sliderHandle.range.decreaseSingleStep()
                                }
                                else
                                {
                                    sliderHandle.range.increaseSingleStep()
                                }
                            }
                        }
                    }

                    // Asset Browser View Options
                    WGPushButton {
                        id: btnAssetBrowserOrientation
                        objectName: "btnAssetBrowserOrientation"
                        iconSource: checked ? "icons/rows_16x16.png" : "icons/columns_16x16.png"
                        checkable: true
                        checked: false

                        tooltip: "Horizontal/Vertical Toggle"

                        onClicked: {
                            if (checked) { //note: The click event changes the checked state before (checked) is tested
                                assetSplitter.state = "VERTICAL"
                            }
                            else
                            {
                                assetSplitter.state = "HORIZONTAL"
                            }
                        }
                    }

                    WGPushButton {
                        id: btnAssetBrowserHideFolders
                        objectName: "btnAssetBrowserHideFolders"
                        iconSource: checked ? "icons/folder_tree_off_16x16.png" : "icons/folder_tree_16x16.png"
                        checkable: true
                        checked: false

                        tooltip: "Hide Folder List"

                        onClicked: {
                            if(checked){
                                leftFrame.visible = false
                            } else {
                                leftFrame.visible = true
                            }
                        }
                    }
                    /*
                    WGToolButton {
                        id: btnUseSelectedAsset
                        objectName: "btnUseSelectedAsset"

                        iconSource: "icons/list_plus_16x16.png"

                        tooltip: "Apply Asset"

                        onClicked: {
                            onUseSelectedAsset()
                        }
                    }*/
                } //end preferences container
            }

            ColumnLayout { // assetBrowser info is reparented within this in checkAssetBrowserWidth()
                id: assetBrowserDoubleLineColumn
                visible:false
                Layout.preferredWidth: 0
                Layout.fillHeight: false
            }

            WGExpandingRowLayout {
                id: assetBrowserInfoSecondLine
                Layout.fillWidth: false
                visible: false
            }
        }


        WGExpandingRowLayout {
            //Filter Box
            id: assetFilter
            Layout.fillWidth: true
            Layout.preferredHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize

            Rectangle {
                id: activeFiltersRect
                Layout.fillWidth: true
                Layout.minimumHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize
                Layout.preferredHeight: childrenRect.height
                color: "transparent"

                WGActiveFilters {
                    id: activeFilters
                    objectName: "activeFilters"
                    anchors {left: parent.left; top: parent.top; right: parent.right}
                    height: childrenRect.height
                    inlineTags: true
                    __splitterChar: ","
                    dataModel: rootFrame.viewModel.data.activeFilters
                }
            }

            // Apply custom filters to data that do not get overridden by
            // the text-based filters. Temporary solution until a more
            // robust filtering system can be added. This will not show
            // if no custom filters are attached, so as to not leave stray
            // and unusable UI elements in the control.
            WGLabel {
                text: "File Type:"
                visible: customContentFiltersList.count > 0
            }

            WGDropDownBox {
                id: customContentFiltersMenu
                Layout.preferredWidth: 150
                visible: customContentFiltersList.count > 0

                model: customContentFiltersList
                currentIndex: 0

                onCurrentIndexChanged: {
                    rootFrame.viewModel.data.currentCustomContentFilter = currentIndex;
                }
            }
        }

        // SplitView that breaks the panel up into two columns with draggable
        // handle. Haven't used this before but seems to work fine.

        SplitView {
            id: assetSplitter
            objectName: "assetSplitter"
            Layout.fillHeight: true
            Layout.fillWidth: true
            orientation: Qt.Horizontal

            Component.onDestruction: {
                //TODO: directly use Preference when supporting dynamically add property for GeneircObject
                addPreference(viewId, "assetViewOrientation", assetSplitter.state );
            }

            states: [
                State {
                    name: "VERTICAL"
                    PropertyChanges { target: assetSplitter; orientation: Qt.Vertical }
                    PropertyChanges { target: leftFrame; height: Math.min(200, Math.round(assetSplitter.height / 3)) }
                },
                State {
                    name: "HORIZONTAL"
                    PropertyChanges { target: assetSplitter; orientation: Qt.Horizontal }
                    PropertyChanges { target: leftFrame; width: Math.min(300, Math.round(assetSplitter.width / 3)) }
                }
            ]

            // TODO Maybe should be a separate WG Component
            handleDelegate: Item {

                // yes this is reversed. Blame the default SplitView for being stupid.
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

            Rectangle {
                // This rectangle is basically an invisible layer ... but for
                // some reason if the first level in a SplitView is a layout,
                // it behaves weirdly with minimumWidths
                id: leftFrame
                objectName: "leftFrame"

                Layout.minimumHeight: 0;
                Layout.minimumWidth: 0;
                height: Math.min(200, Math.round(assetSplitter.height / 3));
                width: Math.min(250, Math.round(assetSplitter.width/3));

                color: "transparent"

                Component.onDestruction: {
                    //TODO: directly use Preference when supporting dynamically add property for GeneircObject
                    addPreference(viewId, "leftFrameVisible", leftFrame.visible ? "true" : "false" );
                    addPreference(viewId, "leftFrameWidth", leftFrame.width );
                    addPreference(viewId, "leftFrameHeight", leftFrame.height );
                }

                // Left Column: Search bar and folder tree
                ColumnLayout {
                    id: parentColumnLayout
                    anchors.fill: parent
                    /*
                    // Filtering
                    WGExpandingRowLayout {
                        id: folderFilterControls
                        Layout.fillWidth: true
                        Layout.preferredHeight: defaultSpacing.minimumRowHeight + defaultSpacing.doubleBorderSize

                        WGPushButton {
                            id: btnOpenAssetLocation
                            iconSource: "icons/search_folder_16x16.png"

                            tooltip: "Collection Options"

                            menu: WGMenu {
                                title: "Collections"
                                MenuItem {
                                    text: "MOCKUP ONLY"
                                }

                                MenuSeparator{}

                                MenuItem {
                                    text: "Collection 1"
                                    checkable: true
                                    checked: true
                                }
                                MenuItem {
                                    text: "Collection 2"
                                    checkable: true
                                    checked: true
                                }
                                MenuItem {
                                    text: "Collection 3"
                                    checkable: true
                                    checked: false
                                }

                                MenuSeparator { }

                                MenuItem {
                                    text: "Edit Current Collection..."
                                }
                                MenuItem {
                                    text: "Delete Current Collection..."
                                }

                                MenuSeparator { }

                                MenuItem {
                                    text: "Create New Collection..."
                                }
                            }
                        }

                        // TODO Make search box search for things
                        WGTextBox {
                            id: folderSearchBox
                            Layout.fillWidth: true
                            Layout.preferredHeight: defaultSpacing.minimumRowHeight
                            placeholderText: "Search"
                        }
                    }*/

                    // TODO Set this up to use tabs for different collections.
                    // This will need a proper TabViewStyle made though

                    WGTreeView {
                        id: folderView
                        objectName: "folderView"
                        model : folderModel
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        columnDelegates : [foldersColumnDelegate]
                        selectionExtension: selector
                        treeExtension: folderTreeExtension
                        backgroundColourMode: uniformRowBackgroundColours
                        lineSeparator: true

                        property Component foldersColumnDelegate:
                            Item {
                                id: folderIconHeaderContainer
                                Image{
                                    id: folderFileIcon
                                    objectName: typeof(itemData.value) != "undefined" ? "folderFileIcon_" + itemData.value : "folderFileIcon"
                                    anchors.verticalCenter: folderIconHeaderContainer.verticalCenter
                                    visible: true
                                    anchors.left: folderIconHeaderContainer.left //itemData.expandIconArea.right
                                    width: sourceSize.width
                                    height: sourceSize.heigth
                                    //TODO: Awaiting type support for icon customisation
                                    source: itemData.hasChildren ? (itemData.expanded ? "icons/folder_open_16x16.png" : "icons/folder_16x16.png") : "icons/file_16x16.png"
                                }
                                Text {
                                    objectName: typeof(itemData.value) != "undefined" ? "Text_" + itemData.value : "Text"
                                    anchors.left: folderFileIcon.right
                                    color: palette.textColor
                                    clip: itemData != null && itemData.component != null
                                    text: itemData != null ? (itemData.value != null ? itemData.value : "") : ""
                                    anchors.leftMargin: folderView.expandIconMargin // TODO no defined error
                                    font.bold: itemData != null && itemData.hasChildren
                                    verticalAlignment: Text.AlignVCenter
                                    anchors.verticalCenter: folderIconHeaderContainer.verticalCenter
                                    elide: Text.ElideRight
                                }
                            }
                    }// TreeView
                        /*
                        Rectangle {
                            id: recentFileHistoryRect
                            color: "transparent"
                            anchors.fill: parent

                            Layout.fillHeight: true
                            Layout.preferredHeight: defaultSpacing.minimumRowHeight
                            Layout.fillWidth: true

                            WGListView {
                                id: recentFileHistoryList

                                anchors.fill: parent

                                model: recentFileHistoryModel
                                enableVerticalScrollBar: true
                                selectionExtension: recentFileHistorySelection
                                columnDelegates: [historyColumnDelegate]
                            }

                            Component {
                                id: historyColumnDelegate

                                Item {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: defaultSpacing.minimumRowHeight
                                    Rectangle {
                                        id: historyIcon

                                        color: "transparent"
                                        width: defaultSpacing.minimumRowHeight

                                        anchors.left: parent.left
                                        anchors.top: parent.top
                                        anchors.bottom: parent.bottom

                                        Image {
                                            source: "icons/file_16x16.png"
                                            anchors.centerIn: parent
                                        }
                                    }

                                    Rectangle {
                                        anchors.left: historyIcon.right
                                        anchors.right: parent.right
                                        anchors.top: parent.top
                                        anchors.bottom: parent.bottom
                                        anchors.margins: 1

                                        color: "transparent"

                                        WGLabel {
                                            text: itemData.value
                                            anchors.fill: parent
                                        }
                                    }
                                }
                            }
                        }*/

                        /* TODO: Favourites functionality should be added later when a preferences system exists to
                                 persist the data. Remove for now, so as to not confuse end-users.
                                 JIRA: NGT-906
                        Tab{
                            title : "Favourites"
                        }*/
                } // End of Column
            } //End LeftFrame

            Item {
                // Could not use a Layout as the first level of a SplitView, it behaves
                // weirdly with minimumWidths

                id: rightFrame
                objectName: "rightFrame"
                Layout.fillHeight: true
                Layout.fillWidth: true

                ColumnLayout {
                    // Right Column: Filters, Files + Assets, Saved Filters & View Options

                    id: fileColumn
                    objectName: "fileColumn"
                    anchors.fill: parent

                    Rectangle {
                        //Assets/Files Frame
                        id: folderContentsRect

                        color: palette.lightPanelColor

                        Layout.fillHeight: true
                        Layout.fillWidth: true

                        clip: true
                        /*
                        Loader {
                            anchors.fill: parent
                            source: showIcons ? assetList : assetGrid
                        }*/

                        // TODO: This should probably be a WGGridView at some point.

                        GridView {
                            id: assetGrid
                            objectName: "assetGrid"
                            visible: showIcons

                            height: folderContentsRect.height
                            width: folderContentsRect.width

                            cellWidth: iconSize < folderContentsRect.width ?
                                           folderContentsRect.width / Math.floor(folderContentsRect.width / iconSize) : iconSize
                            cellHeight: iconSize + 36

                            model: folderContentsModel
                            delegate: folderContentsDelegate

                            snapMode: GridView.SnapToRow


                            highlight: WGHighlightFrame {
                            }

                            highlightMoveDuration: 0

                            WGScrollBar {
                                 id: verticalScrollBar
                                 objectName: "verticalScrollBar"
                                 width: defaultSpacing.rightMargin
                                 anchors.top: assetGrid.top
                                 anchors.right: assetGrid.right
                                 anchors.bottom: assetGrid.bottom
                                 orientation: Qt.Vertical
                                 position: assetGrid.visibleArea.yPosition
                                 pageSize: assetGrid.visibleArea.heightRatio
                                 scrollFlickable: assetGrid
                                 visible: assetGrid.contentHeight > assetGrid.height
                             }

                            onCurrentIndexChanged: {
                                listModelSelection.selectedIndex = model.index(currentIndex);
                            }

                        }

                        Component {
                            id: folderContentsDelegate
                            //Individual grid file/Asset. Height/Width determined by iconSize from iconSizeSlider

                            Item {
                                id: assetEntryRect
                                visible: showIcons
                                width: assetGrid.cellWidth
                                height: assetGrid.cellHeight

                                ColumnLayout {
                                    objectName: typeof(iconLabel.text) != "undefined" ? iconLabel.text : "assetArea"
                                    spacing: 0
                                    anchors.fill: parent

                                    //TODO Replace this with proper thumbnail
                                    Item {
                                        Layout.preferredHeight: iconSize
                                        Layout.preferredWidth: iconSize
                                        Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter

                                        Image {
                                            id: icon_file
                                            anchors.fill: parent
                                            source: {
                                                if (  isDirectory == true )
                                                    return "icons/folder_128x128.png"
                                                else if ( thumbnail != undefined )
                                                    return thumbnail
                                                else
                                                    return "icons/file_128x128.png"
                                            }
                                            Image {
                                                source: statusIcon != undefined ? statusIcon : ""
                                                anchors.left: icon_file.left
                                                anchors.bottom: icon_file.bottom
                                                anchors.leftMargin: iconSize > 32 ? Math.round(iconSize / 12) : 0
                                                anchors.bottomMargin: iconSize > 32 ? Math.round(iconSize / 24) : 0
                                            }
                                        }
                                    }

                                    WGMultiLineText {
                                        id: iconLabel

                                        text: value
                                        horizontalAlignment: Text.AlignHCenter

                                        lineHeightMode: Text.FixedHeight
                                        lineHeight: iconLabelSize + 2

                                        y: -2

                                        Layout.preferredWidth: parent.width - defaultSpacing.rowSpacing * 2
                                        Layout.preferredHeight: defaultSpacing.minimumRowHeight * 2
                                        Layout.alignment: Qt.AlignTop | Qt.AlignHCenter

                                        //height: defaultSpacing.minimumRowHeight * 2

                                        maximumLineCount: {
                                            var lines = 2
                                            if (index == rootFrame.viewModel.currentSelectedAssetIndex){
                                                lines = 3
                                            }
                                            if (iconSize <= 32) {
                                                lines += 1
                                            }
                                            return lines
                                        }
                                        wrapMode: Text.WrapAnywhere

                                        font.pointSize: iconLabelSize

                                        elide: {
                                            if (index == rootFrame.viewModel.currentSelectedAssetIndex){
                                                return Text.ElideNone
                                            } else {
                                                return Text.ElideRight
                                            }
                                        }
                                    }
                                }

                                MouseArea {
                                    id: assetMouseArea
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor

                                    acceptedButtons: Qt.RightButton | Qt.LeftButton

                                    onPressed: {
                                        if(mouse.button == Qt.LeftButton){
                                            assetGrid.currentIndex = index
                                        }
                                    }

                                    onClicked: {
                                        if(mouse.button == Qt.RightButton){
                                            assetGrid.currentIndex = index
                                        }
                                        fileContextMenu.onClicked(mouse)
                                    }

                                    onDoubleClicked: {
                                        if(mouse.button == Qt.LeftButton){
                                            assetGrid.currentIndex = index
                                            onUseSelectedAsset()
                                        }
                                    }
                                }
                            }
                        }

                        WGListView {
                            id: assetList
                            visible: !showIcons

                            anchors.fill: parent

                            model: folderContentsModel
                            selectionExtension: listModelSelection
                            columnDelegates: [columnDelegate]

                            onRowClicked:{
                                fileContextMenu.onClicked(mouse)
                            }

                            onRowDoubleClicked: {
                                if(mouse.button == Qt.LeftButton) {
                                    onUseSelectedAsset()
                                }
                            }

                            onReturnPressed: {
                                // Select the current asset when the enter key is pressed
                                onUseSelectedAsset();
                            }
                        }

                        Component {
                            id: columnDelegate

                            Item {
                                objectName: "showIcons"
                                visible: !showIcons
                                Layout.fillWidth: true
                                Layout.preferredHeight: defaultSpacing.minimumRowHeight
                                Item {
                                    id: fileIcon

                                    width: defaultSpacing.minimumRowHeight

                                    anchors.left: parent.left
                                    anchors.top: parent.top
                                    anchors.bottom: parent.bottom

                                    Image {
                                        source: itemData.typeIcon != "" ? itemData.typeIcon : "icons/file_16x16.png"
                                        anchors.centerIn: parent
                                    }

                                    Image {
                                        source: itemData.statusIcon != undefined ? itemData.statusIcon : ""
                                        anchors.centerIn: parent
                                    }
                                }

                                Item {
                                    anchors.left: fileIcon.right
                                    anchors.right: parent.right
                                    anchors.top: parent.top
                                    anchors.bottom: parent.bottom
                                    anchors.margins: 1

                                    WGLabel {
                                        text: itemData.value
                                        anchors.fill: parent
                                    }
                                }
                            }
                        }

                    } //Asset Icon Frame
                } //Right Hand Column Layout
            } //RightFrame
        } //SplitView
    }

    /*! Deprecated */
    property alias folderHistoryIndices: rootFrame.__folderHistoryIndices

    /*! Deprecated */
    property alias currentFolderHistoryIndex: rootFrame.__currentFolderHistoryIndex

    /*! Deprecated */
    property alias maxFolderHistoryIndices: rootFrame.__maxFolderHistoryIndices
}
