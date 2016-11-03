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
    contentWidth: contentItem.childrenRect.width + scrollViewError
    header: depth == 0 ? view.header : null
    footer: depth == 0 ? view.footer : null
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
    readonly property var scrollViewError: view.clamp ? 0 : 1

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

        Connections {
            target: view.selectionModel
            onCurrentChanged: {
                if( view.selectionModel.currentIndex === modelIndex  ) {
                    view.parent.currentItem = itemRow.parent;
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

        Item {
            id: childItems
            anchors.top: itemRow.bottom

            Loader {
                active: hasChildren && expanded

                Component.onCompleted: {
                    setSource("WGTreeViewBase20.qml", {
                        "width": Qt.binding( function() { return ( typeof treeViewBase !== "undefined" ) ? treeViewBase.width - childItems.x : 0 } ),
                        "height": Qt.binding( function() { return ( typeof treeViewBase !== "undefined" ) ? treeViewBase.height - childItems.y + childItems.height : 0 } ),
                        "interactive": false,
                        "view": Qt.binding( function() { return treeViewBase.view } ),
                        "model": Qt.binding( function() { return childModel } ),
                        "depth": Qt.binding( function() { return treeViewBase.depth + 1 } ),

                        "__onItemPressed": function(mouse, itemIndex, rowIndex) { treeViewBase.itemPressed(mouse, itemIndex, rowIndex) },
                        "__onItemClicked": function(mouse, itemIndex, rowIndex) { treeViewBase.itemClicked(mouse, itemIndex, rowIndex) },
                        "__onItemDoubleClicked": function(mouse, itemIndex, rowIndex) { treeViewBase.itemDoubleClicked(mouse, itemIndex, rowIndex) },
                    })

                    childItems.width = Qt.binding( function() { return active ? item.contentWidth : 0 } )
                    childItems.height = Qt.binding( function() { return active ? item.contentHeight : 0 } )
                }
            }
        }
    }
}
