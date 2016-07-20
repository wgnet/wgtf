import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQml.Models 2.2
import WGControls 2.0

ListView {
    id: treeViewBase

    property var view
    property real depth: 0

    property var __onItemPressed: function(mouse, itemIndex, rowIndex) {}
    property var __onItemClicked: function(mouse, itemIndex, rowIndex) {}
    property var __onItemDoubleClicked: function(mouse, itemIndex, rowIndex) {}

    signal itemPressed(var mouse, var itemIndex, var rowIndex)
    signal itemClicked(var mouse, var itemIndex, var rowIndex)
    signal itemDoubleClicked(var mouse, var itemIndex, var rowIndex)

    onItemPressed: __onItemPressed(mouse, itemIndex, rowIndex)
    onItemClicked: __onItemClicked(mouse, itemIndex, rowIndex)
    onItemDoubleClicked: __onItemDoubleClicked(mouse, itemIndex, rowIndex)

    headerPositioning: ListView.OverlayHeader
    footerPositioning: ListView.OverlayFooter
    contentWidth: contentItem.childrenRect.width + scrollViewError

    // This workaround is needed until the standard QML ScrollView is fixed.
    readonly property var scrollViewError: view.clamp ? 0 : 1

    header: depth == 0 ? view.header : null
    footer: depth == 0 ? view.footer : null

    delegate: Item {
        height: childrenRect.height
        width: childrenRect.width

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
                        "interactive": false,
                        "width": Qt.binding( function() { return treeViewBase.width - childItems.x } ),
                        "height": Qt.binding( function() { return treeViewBase.height - childItems.y + treeViewBase.contentY } ),

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
