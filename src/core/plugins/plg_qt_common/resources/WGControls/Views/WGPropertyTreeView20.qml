import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2

import WGControls 2.0
import WGControls.Views 2.0

/*!
 \ingroup wgcontrols
*/
WGTreeView {
    id: propertyTreeView
    WGComponent { type: "WGPropertyTreeView20" }

    ComponentExtension {
        id: componentExtension
    }

    extensions: [componentExtension]

    property Component propertyDelegate: Loader {
        width: parent.width
        height: typeof itemData.component != 'undefined' ? (item.height > 0 ? item.height : defaultSpacing.minimumRowHeight)
                                                         : defaultSpacing.minimumRowHeight
        sourceComponent: itemData.component
    }

    columnDelegates: [columnDelegate, propertyDelegate]
    columnSpacing: 1

	property var proxyModel: WGColumnLayoutProxy {
		id: columnLayoutProxy
		columnSequence: [0, 0]
	}

	property alias propertyModel: propertyTreeView.model
	property alias model: columnLayoutProxy.sourceModel

	propertyModel: proxyModel

    Dialog {
        id: keyDialog
        modality: Qt.ApplicationModal
        standardButtons: StandardButton.Ok | StandardButton.Cancel

        property var onAcceptedFunc: null

        Row {
            width: childrenRect.width
            height: childrenRect.height
            spacing: 5

            WGLabel{
                id: label
                text: "Key Value:"
            }

            WGTextBox {
                id: input
                width: 120
                height: label.height
            }
        }

        onAccepted: {
            if (onAcceptedFunc != null) {
                onAcceptedFunc(input.text)
            }
            onAcceptedFunc = null
        }

        onRejected: {
            onAcceptedFunc = null
        }
    }

    WGContextMenu {
        id: contextMenu
        path: "WGPropertyTreeMenu"

        WGAction {
            actionId: "WGPropertyTreeMenu|.Push"

            onTriggered: {
                var collection = item.value
                if (collection.isMapping()) {
                    keyDialog.onAcceptedFunc = function(key) {
                        if (key == "") {
                            return
                        }
                        collection.insertItem(key)
                    }
                    keyDialog.open()
                }
                else {
                    var index = collection.count()
                    collection.insertItem(index)
                }
            }

            property var item: contextMenu.contextObject

            onItemChanged: {
                if (item != null && item.isCollection) {
                    active = true
                }
                else {
                    active = false
                }
            }
        }

        WGAction {
            actionId: "WGPropertyTreeMenu|.Pop"

            onTriggered: {
                var collection = item.value
                if (collection.isMapping()) {
                    var index = collection.count() - 1
                    var childItem = propertyTreeView.internalModel.item(index, 0, item)
                    var key = childItem.key
                    collection.removeItem(key)
                }
                else {
                    var index = collection.count() - 1
                    collection.removeItem(index)
                }
            }

            property var item: contextMenu.contextObject

            onItemChanged: {
                if (item != null && item.isCollection) {
                    var collection = item.value
                    active = collection.count() > 0
                }
                else {
                    active = false
                }
            }
        }

        WGAction {
            actionId: "WGPropertyTreeMenu|.Insert"

            onTriggered: {
                var collection = parentItem.value
                if (collection.isMapping()) {
                    keyDialog.onAcceptedFunc = function(key) {
                        if (key == "") {
                            return
                        }
                        collection.insertItem(key)
                    }
                    keyDialog.open()
                }
                else {
                    var index = propertyTreeView.internalModel.rowIndex(item)
                    collection.insertItem(index)
                }
            }

            property var item: contextMenu.contextObject
            property var parentItem: propertyTreeView.internalModel.parent(item)

            onParentItemChanged: {
                if (parentItem != null && parentItem.isCollection == true) {
                    active = true
                }
                else {
                    active = false
                }
            }
        }

        WGAction {
            actionId: "WGPropertyTreeMenu|.Remove"

            onTriggered: {
                var collection = parentItem.value
                if (collection.isMapping()) {
                    var key = item.key
                    collection.removeItem(key)
                }
                else {
                    var index = propertyTreeView.internalModel.rowIndex(item)
                    collection.removeItem(index)
                }
            }

            property var item: contextMenu.contextObject
            property var parentItem: propertyTreeView.internalModel.parent(item)

            onParentItemChanged: {
                if (parentItem != null && parentItem.isCollection == true) {
                    active = true
                }
                else {
                    active = false
                }
            }
        }
    }

    onItemClicked: {
        if (mouse.button != Qt.RightButton) {
            return
        }

        contextMenu.contextObject = internalModel.indexToItem(rowIndex)
        contextMenu.popup()
    }
}
