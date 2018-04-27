import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQml.Models 2.2
import WGControls 2.0
import WGControls.Views 2.0

/** Internal WGTreeView specific logic.
\ingroup wgcontrols */
ListView {
    id: treeViewBase
    WGComponent { type: "WGTreeViewBase20" }

    headerPositioning: ListView.OverlayHeader
    footerPositioning: ListView.OverlayFooter
    contentWidth: view ? view.totalColumnsWidth + scrollViewError : 0
    header: view && depth == 0 ? view.header : null
    footer: view && depth == 0 ? view.footer : null
    highlightFollowsCurrentItem: false

    /** Link to the common view component.*/
    property var view
    /** Specifies the level down a tree hierarchy.*/
    property real depth: 0
    /** Internal function to forward signals from lower levels to this itemPressed signal.*/
    property var __onItemPressed: function(mouse, itemIndex, rowIndex) {}
    /** Internal function to forward signals from lower levels to this itemClicked signal.*/
    property var __onItemClicked: function(mouse, itemIndex, rowIndex) {}
    /** Internal function to forward signals from lower levels to this itemDoubleClicked signal.*/
    property var __onItemDoubleClicked: function(mouse, itemIndex, rowIndex) {}
    /** This workaround is needed until the standard QML ScrollView is fixed. It is out by one pixel.*/
    readonly property var scrollViewError: view && view.clamp ? 0 : 1
    /** Specify the level in the tree to automatically expand down to on creation.
        Defaults to 0 (do not automatically expand any level).
        Set to 'maxDepth' to expand the entire tree.*/
    property real expandToDepth: 0
    /** Constant representing maximum depth of the tree hierarchy.*/
    readonly property real maxDepth: Math.pow(2, 31)

    /** Signals that an item received a mouse press.
    \param mouse The mouse data at the time.
    \param itemIndex The index of the item.
    \param rowIndex The index of the row the item is in.*/
    signal itemPressed(var mouse, var itemIndex, var rowIndex)
    /** Signals that this item was clicked.
    \param mouse The mouse data at the time.
    \param itemIndex The index of the item.
    \param rowIndex The index of the row the item is in.*/
    signal itemClicked(var mouse, var itemIndex, var rowIndex)
    /** Signals that this item was double clicked.
    \param mouse The mouse data at the time.
    \param itemIndex The index of the item.
    \param rowIndex The index of the row the item is in.*/
    signal itemDoubleClicked(var mouse, var itemIndex, var rowIndex)

    onItemPressed: __onItemPressed(mouse, itemIndex, rowIndex)
    onItemClicked: __onItemClicked(mouse, itemIndex, rowIndex)
    onItemDoubleClicked: __onItemDoubleClicked(mouse, itemIndex, rowIndex)

    delegate: Item {
        height: childrenRect.height
        width: childrenRect.width
        objectName: "WGTreeItem"
        property alias rowHeight: itemRow.height

        Keys.forwardTo: [treeViewBase]

        Connections {
            target: view
            onCurrentChanged: {
                if( view.selectionModel.currentIndex === modelIndex  ) {
                    view.parent.currentItem = itemRow.parent;
                }
            }
        }

        /* Internal value for saving preferences, save last expanded state */
        property var __expanded: expanded
        /* Internal value for saving preferences, the path to save to */
        property var __preferenceName: {
            if(!hasChildren || typeof viewId == "undefined" ||  viewId == null) {
                return null;
            }
            if(typeof indexPath != "undefined") {
                return typeof indexPath == "string" ? indexPath : Qt.btoa(indexPath);
            }
            if(typeof path != "undefined") {
                return path;
            }
            return null;
        }

        Component.onCompleted: {
            if(__preferenceName != null) {
                var value = getPreferenceValueByName(viewId, __preferenceName);
                if (typeof value != "undefined") {
                    expanded = (value == "true");
                    return;
                }
            }
            if (depth < expandToDepth) {
                expanded = true;
            }
        }

        Connections {
            target: treeViewBase
            onExpandToDepthChanged: {
                if (depth < expandToDepth) {
                    expanded = true;
                }
            }
        }

        Component.onDestruction: {
            if(__preferenceName != null && viewId != null) {
                var value = getPreferenceValueByName(viewId, __preferenceName);
                if (typeof value != "undefined") {
                    addPreference(viewId, __preferenceName, __expanded );
                }
                else {
                    if(__expanded) {
                        addPreference(viewId, __preferenceName, true );
                    }
                }
            }
        }

        Loader {
            id: group
            objectName: "Group"
            sourceComponent: view ? view.style.group : null
			asynchronous: true
			visible: status === group.Ready

            property var groupWidth: itemRow.width
            property var groupHeight: itemRow.height + childItems.height

            property var itemData: model

            property bool isSelected: view ? view.selectionModel.isSelected(modelIndex) : false
            property bool isCurrent: view ? view.selectionModel.currentIndex === modelIndex : false

            property int itemDepth: treeViewBase.depth

            property bool hasChildren: typeof model.hasChildren != "undefined" ? model.hasChildren : false

            Connections {
                target: view
                onSelectionChanged: {
                    group.isSelected = Qt.binding( function() { return view.selectionModel.isSelected(modelIndex); } )
                }
            }
        }

        WGItemRow {
            id: itemRow
            view: treeViewBase.view
            depth: treeViewBase.depth

            onItemPressed: treeViewBase.itemPressed(mouse, itemIndex, modelIndex)
            onItemClicked: treeViewBase.itemClicked(mouse, itemIndex, modelIndex)
            onItemDoubleClicked: treeViewBase.itemDoubleClicked(mouse, itemIndex, modelIndex)
        }

		function getChildDelegate( active )	{
			if(active)
			{
				return WGTreeViewBaseGlobals.getWGTreeViewBaseChildComponent();
			}
			else{
				return null;
			}
		}

        Item {
            id: childItems
            anchors.top: itemRow.bottom

			width : childLoader.status == Loader.Ready && childLoader.active ? childLoader.item.contentWidth : 0;
            height : childLoader.status == Loader.Ready && childLoader.active ? childLoader.item.contentHeight : 0;

            property var bindParentTree : treeViewBase;
            property var bindChildModel : childModel;
            Loader {
				id: childLoader
                active: typeof hasChildren != "undefined" ? hasChildren && expanded : false
				asynchronous: true
				visible: status === Loader.Ready

				sourceComponent: getChildDelegate( active )

            }
            Binding {
                target: childLoader.item
                property : "parentTree";
                value: childItems.bindParentTree;
                when: childLoader.status == Loader.Ready && childLoader.active
            }

            Binding {
                target: childLoader.item
                property : "childParent";
                value: childItems;
                when: childLoader.status == Loader.Ready && childLoader.active
            }

            Binding {
                target: childLoader.item
                property : "childModel";
                value: childItems.bindChildModel;
                when: childLoader.status == Loader.Ready && childLoader.active
            }
        }
    }
}
